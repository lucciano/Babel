#!/usr/bin/perl.exe -w

# s-expression parser (sparse) - A low-level "assembler" for Babel constructs

# TODO: Empty lists, bare values, catch non-existent symbols

# FIXME: 0 causes problems in lists (elsewhere?)

# FIXME: Escaped quotes need to be fixed in both balanced_parse AND encode_values...
#${$string} =~ s/^("(?:[^"\\]|\\")*")\s*//;
#${$string} =~ s/^('(?:[^'\\]|\\')*')\s*//;

use strict;
use feature 'state';
no warnings 'recursion'; # Get "deep recursion" on large .pb files otherwise
use Hash::Pearson16;
use Carp;

use YAML::XS;
use Data::Dumper;

my @asm_file;
my $asm_file;
my $obj_out = [];

my $MWORD_SIZE = 4;
my $HASH_SIZE = 16/$MWORD_SIZE;

my $proj_name = $ARGV[0];

#my $section_name_re = qr/[_A-Za-z][_A-Za-z0-9]*/;
my $section_name_re = qr/[^0-9\[\({\]\)}'"][^\[\({\]\)}'"]*/;
my $sigil_re        = qr/[\*\$&%!#@]/;
my $array_begin     = qr/[\[\({]/;
my $array_end       = qr/[\]\)}]/;
my $quote_char      = qr/['"]/;
my $non_array       = qr/[^\[\({<\]\)}>]/;
my $hex_val         = qr/\s*(0x[A-Fa-f0-9]+)/;

my $double_quote    = qr/"(?:[^"\\]|\\.)*"/;
my $single_quote    = qr/'[^']*'/;
my $double_quote_cap = qr/"((?:[^"\\]|\\.)*)"/;
my $single_quote_cap = qr/'([^']*)'/;
#NOTE: This solution requires double-escaping in the .sp source file,
#which is good enough for dev purposes. For example, to pring "Hello"
#(quotes-inclusive), you will need the following in your .sp file:
#   "\\\"Hello\\\"\n" stdout8
#Also, single-quoted text cannot contain escaped-single-quotes

my $sections = [];
my @asm_files;

while($#ARGV>-1){
    $asm_file = shift @ARGV;
    open ASM_FILE, $asm_file;
    $asm_file = [<ASM_FILE>];
    push @asm_files, $asm_file;
    close ASM_FILE;
}

for $asm_file (@asm_files){
    $asm_file = clean( $asm_file );
    my $parsed = balanced_parse( \$asm_file );
    $parsed = $parsed->[0];
#print Dumper($parsed);
    splice( @{$sections}, 0, 0, @{$parsed} );
}

#print Dumper($sections) and die;

my $obj = encode($sections);

#print Dumper($obj) and die;

create_lst($proj_name, $obj);
create_bbl($proj_name, $obj);
create_c  ($proj_name, $obj);

#########################################################################
#
# CLEANER
#
#########################################################################

#
#
sub clean{
    my $asm_file = shift;
    remove_comments($asm_file);
    remove_ws      ($asm_file);
    return join ' ', @{$asm_file};
}

#
#
sub remove_comments{

    my $asm_file = shift;

    foreach my $line (@{$asm_file}){
        $line =~ s/--.*$//; #Strips a line-comment
    }

}

#
#
sub remove_ws{

    my $asm_file = shift;
    my @asm_file = @{$asm_file};

    chomp for @asm_file;

    splice(@{$asm_file});

    for(@asm_file){
        unless(/^\s*$/){
            push @{$asm_file}, $_;
        }
    }

}

#########################################################################
#
# BALANCED EXPRESSION PARSER
#
#########################################################################

# ( a [ b c { d e f } g ] h i ( j ) k )
sub balanced_parse{

    my $string      = shift;
    my $expression  = [];
    my $temp_string;
    my $eval_string;

    begin_balanced:

        clean_string($string);

        if(${$string} =~ /^$array_begin/){
            ${$string} = substr(${$string}, 1);
            push @{$expression}, balanced_parse($string, $expression);
            goto begin_balanced;
        }
        elsif(${$string} =~ /^($array_end)/){
            ${$string} = substr(${$string}, 1);
            #die if not is_matching_paren($paren_type, $1);
            return $expression;
        }
        elsif(${$string} =~ /^"/){
            #${$string} =~ s/^\s*("[^"]*")\s*//;
            ${$string} =~ s/^\s*($double_quote)\s*//;
            $eval_string = "\$temp_string = $1;";
            die unless eval($eval_string); #FIXME: fails with string "0" (perl false)
            push @{$expression}, "\"$temp_string\"";
            goto begin_balanced;
        }
        elsif(${$string} =~ /^'/){
#print "." . substr(${$string}, 0, 20) . ".\n";
            ${$string} =~ s/^\s*($single_quote)\s*//;
            push @{$expression}, "$1";
            goto begin_balanced;
        }
        elsif(${$string} =~ /^\s*[^\s\(\)]+\s*/){
            ${$string} =~ s/^\s*([^\s\(\)]+)\s*//;
            push @{$expression}, "$1";
            goto begin_balanced;
        }
        elsif(${$string} eq ''){
            return $expression;
        }
        else{
            print "Error: .${$string}.\n" and die;
        }

}


sub clean_string{
    my $string = shift;
    carp if(!ref($string));

    ${$string} =~ s/^\s*//;
    ${$string} =~ s/\s*$//;
}

sub get_paren_type {
    for(shift){
        /\(/ and return "LIST_PAREN";
        /\)/ and return "LIST_PAREN";
        /\[/ and return "ARRAY_PAREN";
        /\]/ and return "ARRAY_PAREN";
        /{/  and return "CODE_LIST_PAREN";
        /}/  and return "CODE_LIST_PAREN";
        #/</  and return "BRACKET_PAREN";
        #/>/  and return "BRACKET_PAREN";
    }
}

sub is_matching_paren{
    my ($open_paren, $close_paren) = @_;
    return 1 if (get_paren_type($open_paren) eq get_paren_type($close_paren));
    return 0;
}

#
#
sub tokenize{

    my $string = shift;
    my $eval_string = "";
    my $temp_string = "";

    my @token_list = ();

    $string =~ s/^\s*//;

    while(length($string) > 0){
        if($string =~ /^\s*$double_quote\s*/){
            $string =~ s/^\s*($double_quote)\s*//;
            $eval_string = "\$temp_string = $1;";
            die unless eval($eval_string);
            push @token_list, "\"$temp_string\"";
        }
        elsif($string =~ /^\s*$single_quote\s*/){
            $string =~ s/^\s*($single_quote)\s*//;
            push @token_list, "$1";
        }
        elsif($string =~ /^\s*[\S]+\s*/){
            $string =~ s/^\s*([\S]+)\s*//;
            push @token_list, "$1";
        }
        else{
            print "Error: .$string.\n" and die;
        }
        $string =~ s/^\s*//;
    }

    return @token_list;

}

#########################################################################
#
# ENCODER
#
#########################################################################

#
#
sub encode{

    my $parse_tree = shift;

    #$parse_tree = $parse_tree->[0];

    #print Dumper($parse_tree) and die;

    my $symbol_table = {};
#    $obj->{code} = [];

    # build symbol-table
    foreach my $named_section (@{$parse_tree}){
        #print Dumper($named_section) and die;
        $symbol_table->{$named_section->[0]} = $named_section->[1];
    }

    #print Dumper($symbol_table) and die;

    my $offset = 0;

    #$obj->{code}{'root'} = [];
    return encode_section($symbol_table, {}, 'root', \$offset);

    #return $obj;

}

#
#
sub encode_section{

    my ($symbol_table, 
        $addr_lut, 
        $section_name, 
        $offset) = @_;

    if($section_name eq "nil"){ # If nil is reference prior to creation of the first list...
        return create_nil($addr_lut, $offset);
    }

    $addr_lut->{$section_name} = ($$offset+1) * $MWORD_SIZE;

    return encode_tree($symbol_table, $addr_lut, $section_name, $offset, $symbol_table->{$section_name});

}

#
#
sub encode_tree{

    my ($symbol_table, 
        $addr_lut,
        $section_name, 
        $offset,
        $sub_tree) = @_;

    return unless defined $sub_tree;

    if   ($sub_tree->[0] eq 'val'){ #values
        return encode_values        ($sub_tree, $offset, 0);
    }
    elsif($sub_tree->[0] eq 'ptr'){ #pointers
        return encode_pointers      ($symbol_table, $addr_lut, $section_name, $offset, $sub_tree);
    }
    elsif($sub_tree->[0] eq 'tag'){ #tagged-list
        return encode_tagged_list   ($symbol_table, $addr_lut, $section_name, $offset, $sub_tree);
    }
    elsif($sub_tree->[0] eq 'list'){ #list
        return encode_list          ($symbol_table, $addr_lut, $section_name, $offset, $sub_tree);
    }
    elsif($sub_tree->[0] eq 'hash'){ #hash
        return encode_hash          ($sub_tree, $offset, 0);
    }
    elsif($sub_tree->[0] eq 'oper'){ #operator
        return encode_values        ($sub_tree, $offset, 0);
    }
    elsif($sub_tree->[0] eq 'ref'){ #reference
        return encode_ref   ($symbol_table, $addr_lut, $section_name, $offset, $sub_tree);
    }
    elsif($sub_tree->[0] eq 'code'){ #code
        return encode_code_list    ($symbol_table, $addr_lut, $section_name, $offset, $sub_tree);
    }
    else{
        print "@{$symbol_table->{$section_name}}\n";
        print "Unrecognized list type: $symbol_table->{$section_name}->[0]\n" and die;
    }

}

#
#
sub encode_values{

    my ($sub_tree, 
        $offset,
        $single_mode) = @_;

    my $eval_string;
    my @str_vec;

    my @values = @{$sub_tree};
    shift @values;

    #$$offset += ($#values+2);
    $$offset += 1;

    my $value_list = [];

    foreach my $value (@values){

        if($value =~ /^(0)$/ or $value =~ /^(\s*-?[1-9][0-9]*)$/ or $value =~ /^(\s*0+)[^x]$/ ){
            push @{$value_list}, 1+$1-1; #FORCE Perl to treat it as numeric
            $$offset += 1;
        }
        elsif($value =~ /^\s*$hex_val$/){
            push @{$value_list}, hex($1);
            $$offset += 1;
        }
        #elsif($value =~ /^\s*"[^"]*"$/){
        elsif($value =~ /^\s*$double_quote$/){

            $eval_string = "\$value = $value;";
            die unless eval($eval_string);
            #$value = "\"$value\"";

            @str_vec = str2vec($value);
            push @{$value_list}, @str_vec;
            $$offset += ($#str_vec+1);
            
        }
#        elsif($value =~ /^\s*'([^']*)'$/){
        elsif($value =~ /^\s*$single_quote_cap$/){
            @str_vec = str2vec($1);
            push @{$value_list}, @str_vec;
            $$offset += ($#str_vec+1);
        }
        elsif($value =~ /^($section_name_re)$/){
            print ".$value.\n";
            die;
        }
        else{
            print "Error: .$value.\n" and die;
        }

        last if $single_mode;

    }

    unshift @{$value_list}, $MWORD_SIZE * ($#{$value_list}+1);

    return $value_list;

}


#
#
sub encode_hash{

    my ($sub_tree, 
        $offset) = @_;

    my $eval_string;
    my @str_vec;

    my @values = @{$sub_tree};
    shift @values;

    #$$offset += ($#values+2);
    $$offset += 1;

    my $value_list = [];
    my $value = $values[0];
    my @hash = ();

#    if($value =~ /^\s*"[^"]*"$/){
    if($value =~ /^\s*$double_quote$/){

        $eval_string = "\$value = $value;";
        die unless eval($eval_string);

        @hash = bytes_to_mwords(Hash::Pearson16::pearson16_hash($value));
        push @{$value_list}, @hash;
        $$offset += ($#hash+1);
        
    }
    elsif($value =~ /^\s*$single_quote$/){

        @hash = bytes_to_mwords(Hash::Pearson16::pearson16_hash($1));
        push @{$value_list}, @hash;
        $$offset += ($#hash+1);
        
    }
    else{
        print "Error: .$value.\n" and die;
    }

    unshift @{$value_list}, $MWORD_SIZE * ($#{$value_list}+1);

    return $value_list;

}


#
#
sub encode_pointers{

    my ($symbol_table, 
        $addr_lut,
        $section_name, 
        $offset,
        $sub_tree) = @_;

    my $encoded;

    my @pointers = @{$sub_tree};
    shift @pointers;

    $$offset += ($#pointers+2);

    my $pointer_list = [];
    push @{$pointer_list}, -1*$MWORD_SIZE*($#pointers+1);

    for(0..$#pointers){
        push @{$pointer_list},0xdeadbeef;
    }

    my $i = 1;
    foreach my $pointer (@pointers){

        if(ref($pointer) eq ""){
        
            if(is_value($pointer)){
                $pointer_list->[$i] = $MWORD_SIZE*($$offset+1);
                $encoded = encode_values(["val", $pointer], $offset, 1);
                push (@{$pointer_list}, $_) for (@{$encoded});
                #printf("A:%x ", $$offset*$MWORD_SIZE);
            }
            else{
                #printf("B:%x ", $$offset*$MWORD_SIZE);
                unless(exists $addr_lut->{$pointer}){
                    $encoded = encode_section($symbol_table, $addr_lut, $pointer, $offset);
                    push (@{$pointer_list}, $_) for (@{$encoded});
                }
                $pointer_list->[$i] = $addr_lut->{$pointer};
            }
        
        }
        else{
            #printf("C:%x ", $$offset*$MWORD_SIZE);
            $pointer_list->[$i] = $MWORD_SIZE*($$offset+1);
            $encoded = encode_tree($symbol_table, $addr_lut, $section_name, $offset, $pointer);
            push (@{$pointer_list}, $_) for (@{$encoded});
        }
        $i++;

    }

    return $pointer_list;

}

#
#
sub encode_list{

    my ($symbol_table, 
        $addr_lut,
        $section_name, 
        $offset,
        $sub_tree) = @_;

    my $encoded;
    my ($car, $cdr);

    my @elements = @{$sub_tree};
    shift @elements;

    #$$offset += ($#elements+2);

    my $element_list = [];
    #push @{$element_list}, -1*$MWORD_SIZE*($#elements+1);

    foreach my $element (@elements){

        $$offset += 3;
        push @{$element_list}, -2*$MWORD_SIZE;
        for(1..2){
            push @{$element_list}, 0xdeadbeef;
        }

        $car = $#{$element_list}-1;
        $cdr = $#{$element_list};

        if(ref($element) eq ""){

            if(is_value($element)){
                #print "value: " . is_value($element);
                $element_list->[$car] = $MWORD_SIZE*($$offset+1);
                $encoded = encode_values(["val", $element], $offset, 1);
                push (@{$element_list}, $_) for (@{$encoded});
                #printf("A:%x ", $$offset*$MWORD_SIZE);
            }
            else{
                unless(exists $addr_lut->{$element}){
                    $encoded = encode_section($symbol_table, $addr_lut, $element, $offset);
                    push (@{$element_list}, $_) for (@{$encoded});
                }
                $element_list->[$car] = $addr_lut->{$element};
            }

        }
        else{
            $element_list->[$car] = $MWORD_SIZE*($$offset+1);
            $encoded = encode_tree($symbol_table, $addr_lut, $section_name, $offset, $element);
            push (@{$element_list}, $_) for (@{$encoded});
        }
        $element_list->[$cdr] = ($$offset+1) * $MWORD_SIZE;

    }

    if(!exists $addr_lut->{nil}){
        $encoded = create_nil($addr_lut, $offset);
        push (@{$element_list}, $_) for (@{$encoded});
    }

    $element_list->[$cdr] = $addr_lut->{nil};

    return $element_list;

}

#
#
sub encode_tagged_list{

    my ($symbol_table, 
        $addr_lut,
        $section_name, 
        $offset,
        $sub_tree) = @_;

    my $encoded;
    my $car;

    my @elements = @{$sub_tree};
    shift @elements;

    my $element_list = [];

    my $plain_tag = shift @elements;
    my $tag;

    die if ($#elements > 0); # Only one item can be tagged at a time

    push @{$element_list}, 0;

#    if(    $plain_tag =~ /^\s*"((?:[^"\\]|\\.)*)"$/ 
#        or $plain_tag =~ /^\s*'([^']*)'$/){
    if($plain_tag =~ /^\s*$double_quote_cap$/ or $plain_tag =~ /^\s*$single_quote_cap$/){
        push @{$element_list}, bytes_to_mwords(Hash::Pearson16::pearson16_hash($1));
    }
    else{
        print "Error: .$plain_tag.\n" and die;
    }

    $$offset += 7;

    my $element = $elements[0];

    push @{$element_list}, -1 * $MWORD_SIZE;
    push @{$element_list}, 0xdeadbeef;

    $car = $#{$element_list};

    if(ref($element) eq ""){
        unless(exists $addr_lut->{$element}){
            $encoded = encode_section($symbol_table, $addr_lut, $element, $offset);
            push (@{$element_list}, $_) for (@{$encoded});
        }
        $element_list->[$car] = $addr_lut->{$element};
    }
    else{
        $element_list->[$car] = $MWORD_SIZE*($$offset+1);
        $encoded = encode_tree($symbol_table, $addr_lut, $section_name, $offset, $element);
        push (@{$element_list}, $_) for (@{$encoded});
    }

    if(!exists $addr_lut->{nil}){
        $encoded = create_nil($addr_lut, $offset);
        push (@{$element_list}, $_) for (@{$encoded});
    }

    return $element_list;

}


#
#
sub encode_ref{

    my ($symbol_table, 
        $addr_lut,
        $section_name, 
        $offset,
        $sub_tree) = @_;

    my $encoded;
    my ($car, $cdr);

    my @elements = @{$sub_tree};
    shift @elements;

    my @list = @elements;
    unshift(@list, "list");

    my $element_list = [];

    push @{$element_list}, 0;
    push @{$element_list}, bytes_to_mwords(Hash::Pearson16::pearson16_hash("/babel/tag/ref"));
    push @{$element_list}, -2 * $MWORD_SIZE;
    push @{$element_list}, 0xdeadbeef;
    push @{$element_list}, 0xdeadbeef;

    $car = $#{$element_list}-1;
    $cdr = $#{$element_list};

    $$offset += 8;

    my $offset_save = $$offset+1;

    $encoded = encode_list($symbol_table, $addr_lut, $section_name, $offset, \@list);
    push (@{$element_list}, $_) for (@{$encoded});

    $element_list->[$car] = $offset_save * $MWORD_SIZE;

    if(!exists $addr_lut->{nil}){
        $encoded = create_nil($addr_lut, $offset);
        push (@{$element_list}, $_) for (@{$encoded});
    }

    $element_list->[$cdr] = $addr_lut->{nil};

#    printf("%08x\n", $_) for @{$element_list};
#    die;

    return $element_list;

}


#
#
sub encode_code_list{

    my ($symbol_table, 
        $addr_lut,
        $section_name, 
        $offset,
        $sub_tree) = @_;

    my $encoded;
    my ($car, $cdr);

    my @elements = @{$sub_tree};
    shift @elements;

    #$$offset += ($#elements+2);

    my $element_list = [];
    #push @{$element_list}, -1*$MWORD_SIZE*($#elements+1);

    foreach my $element (@elements){

        $$offset += 3;
        push @{$element_list}, -2*$MWORD_SIZE;
        for(1..2){
            push @{$element_list}, 0xdeadbeef;
        }

        $car = $#{$element_list}-1;
        $cdr = $#{$element_list};

        if(ref($element) eq ""){

            if(is_value($element)){
                #print "value: " . is_value($element);
                $element_list->[$car] = $MWORD_SIZE*($$offset+1);
                $encoded = encode_pointers($symbol_table, $addr_lut, $section_name, $offset, ["ptr", ["val", $element]]);
                push (@{$element_list}, $_) for (@{$encoded});
            }
            else{ #symbol
                if($symbol_table->{$element}[0] and !($symbol_table->{$element}[0] eq "oper")){
                    $element_list->[$car] = ($$offset+1) * $MWORD_SIZE;
                    $$offset += 2;
                    $car = $#{$element_list};
                    push @{$element_list}, -1*$MWORD_SIZE;
                    #push @{$element_list}, ($$offset+1) * $MWORD_SIZE;

                    unless(exists $addr_lut->{$element}){
                        push @{$element_list}, ($$offset+1) * $MWORD_SIZE;
                        $encoded = encode_section($symbol_table, $addr_lut, $element, $offset);
                        push (@{$element_list}, $_) for (@{$encoded});
                    }
                    else{
                        push @{$element_list}, $addr_lut->{$element};
                    }

                    $element_list->[$car] = $addr_lut->{$element};
                }
                else{

                    unless(exists $addr_lut->{$element}){
                        $encoded = encode_section($symbol_table, $addr_lut, $element, $offset);
                        push (@{$element_list}, $_) for (@{$encoded});
                    }

                    $element_list->[$car] = $addr_lut->{$element};

                }

            }

        }
        else{
            $element_list->[$car] = ($$offset+1) * $MWORD_SIZE;
            $$offset += 2;
            $car = $#{$element_list};
            push @{$element_list}, -1*$MWORD_SIZE;
            push @{$element_list}, ($$offset+1) * $MWORD_SIZE;

            $element_list->[$car] = $MWORD_SIZE*($$offset+1);
            $encoded = encode_tree($symbol_table, $addr_lut, $section_name, $offset, $element);
            push (@{$element_list}, $_) for (@{$encoded});
        }
        $element_list->[$cdr] = ($$offset+1) * $MWORD_SIZE;

    }

    if(!exists $addr_lut->{nil}){
        $encoded = create_nil($addr_lut, $offset);
        push (@{$element_list}, $_) for (@{$encoded});
    }

    $element_list->[$cdr] = $addr_lut->{nil};

    return $element_list;

}


#
#
sub is_value{

    my $value = shift;

    if( $value =~ /^\s*0$/ or
        $value =~ /^\s*-?[1-9][0-9]*$/ or 
        #$value =~ /^\s*0+[^x]$/ or
        #$value =~ /^\s*0x[A-Fa-f0-9]+$/ or
        $value =~ /^\s*$hex_val$/ or
        $value =~ /^\s*$double_quote$/ or
        $value =~ /^\s*$single_quote$/){
        return 1;
    }

#    if( $value =~ /^\s*0$/ or
#        $value =~ /^(\s*-?[1-9][0-9]*)$/ or 
#        $value =~ /^(\s*0+)[^x]$/ or
#        $value =~ /^(\s*0x[A-Fa-f0-9]+)$/ or
#        $value =~ /^\s*"[^"]*"$/ or
#        $value =~ /^\s*'([^']*)'$/){
#        return 1;
#    }

    return 0;

}


#
#
sub create_nil{

    my ($addr_lut, $offset) = @_;

    $addr_lut->{nil} = ($$offset+1) * $MWORD_SIZE;
#    $$offset += 8;
    $$offset += 7;

#    return [ 0, 0x3023f4e7, 0x8c2f644d, 0x71cf647b, 0xe974b23a, -2*$MWORD_SIZE, $addr_lut->{nil}, $addr_lut->{nil} ];
    return [ 0, 0x3023f4e7, 0x8c2f644d, 0x71cf647b, 0xe974b23a, -1*$MWORD_SIZE, $addr_lut->{nil} ];

}


#########################################################################
#
# FILE OUTPUT
#
#########################################################################

#
#
sub create_lst{

    my $lst_file = shift;
    my $obj_root = shift;
#    my $obj = shift;
    my $i = 0;

    open LST_FILE, ">${lst_file}.lst" or die "Couldn't create .lst: $!";

    for my $word (@{$obj_root}) {
        printf LST_FILE ("%04x %08x\n", $MWORD_SIZE * $i++, $word);
    }

#    foreach (sort keys %{$obj}){
#        if(defined $obj->{addr}{$_}){
##            my $op = $_;
##            $op =~ s/%/%%/g;
###            printf("$op\n");
#            printf LST_FILE ("%08x $_\n", $obj->{addr}{$_});
#        }
#    }
#
    close LST_FILE;

}

#
#
sub create_bbl{

    my $bbl_file = shift;
    my $obj_root = shift;

    open BBL_FILE, ">${bbl_file}.bbl" or die "Couldn't create .bbl: $!";
    binmode BBL_FILE;

    for(@{$obj_root}){
        print  BBL_FILE pack("V",  $_);
    }

    close BBL_FILE;

}

#
#
sub create_c{

    my $c_file = shift;
    my $obj_root = shift;


    my $BBL_SIZE = $#{$obj_root} + 1;

my $H_FILE_TEXT = <<"END_H_FILE_TEXT";
// construct.sp.h
//

#ifndef CONSTRUCT_SP_H
#define CONSTRUCT_SP_H

#include "babel.h"

#define BBL_SIZE $BBL_SIZE

extern const mword bbl[BBL_SIZE];

#endif //CONSTRUCT_SP_H

// Clayton Bauman 2013
END_H_FILE_TEXT

    open H_FILE,   ">${c_file}.h" or die "Couldn't create .h: $!";

    print H_FILE $H_FILE_TEXT;

    close H_FILE;


    open C_FILE,   ">${c_file}.c" or die "Couldn't create .c: $!";

my $C_FILE_TEXT = <<'END_C_FILE_TEXT';
// construct.sp.c
//

#include "construct.sp.h"

const mword bbl[] = {
END_C_FILE_TEXT


    #printf C_FILE ("#define BBL_SIZE %d\nunsigned bbl[BBL_SIZE] = {", $#{$obj_root} + 1);
    print C_FILE $C_FILE_TEXT; #("#include \"construct.sp.h\"\n\nunsigned bbl[BBL_SIZE] = {");

    my $i = 0;
    for(@{$obj_root}){
        if($i++ % 8 == 0){
            print C_FILE "\n    ";
        }
        printf   C_FILE ("0x%08x, ", $_ & 0xffffffff);
        #$i++;
    }

    print C_FILE "\n};\n\n";
    print C_FILE "// Clayton Bauman 2013\n";
    close C_FILE;

}

#########################################################################
#
# UTILITIES
#
#########################################################################

sub bytes_to_mwords{

    my @bytes = @_;
    my @mwords;
    my $temp=0;

    for(my $i=0; $i<=$#bytes; $i++){
#        printf("%02x ", $bytes[$i]);
        if( (($i % $MWORD_SIZE) == 0) and ($i != 0) ){
            push @mwords, $temp;
            $temp=0;
        }
        $temp = $temp | ( $bytes[$i] << (($i % $MWORD_SIZE) * 8) ); #8 bits per byte
    }
    push @mwords, $temp;
#    printf("%08x ", $_) for @mwords;
#    die;

    return @mwords;

}

sub dump_obj{

    my $obj = shift;
    my $i = 0;
    
    for my $word (@{$obj}) {
        printf("%04x %08x\n", $i++, $word);
    }

}

sub str2vec {

    my $str = shift;

    my $index = 0;
    my $my_len;
    my @vec;

    while(1){
        $my_len = length( substr( $str, $index ) );
        if($my_len == 0){
#            push @vec, 0xffffffff;
            push @vec, 0x00000000;
            last;
        }
        elsif($my_len == 1){
            push @vec, ord( substr( $str, $index, 1 ) );
            push @vec, 0xffffff00;
#            push @vec, 0x000000ff;
            last;
        }
        elsif($my_len == 2){
            push @vec, 
                (
                     ord( substr( $str, $index, 1 ) ) 
                    |
                    (ord( substr( $str, $index+1, 1 ) ) << 8)
                )
                ;
            push @vec, 0xffff0000;
#            push @vec, 0x0000ffff;
            last;
        }
        elsif($my_len == 3){
            push @vec, 
                (
                     ord( substr( $str, $index, 1 ) ) 
                    |
                    (ord( substr( $str, $index+1, 1 ) ) << 8)
                    |
                    (ord( substr( $str, $index+2, 1 ) ) << 16)
                )
                ;
            push @vec, 0xff000000;
#            push @vec, 0x00ffffff;
            last;
        }
        else{
            push @vec, 
                (
                     ord( substr( $str, $index, 1 ) ) 
                    |
                    (ord( substr( $str, $index+1, 1 ) ) << 8)
                    |
                    (ord( substr( $str, $index+2, 1 ) ) << 16)
                    |
                    (ord( substr( $str, $index+3, 1 ) ) << 24)
                )
                ;
            $index+=4;
        }

    }

    return @vec;

}



