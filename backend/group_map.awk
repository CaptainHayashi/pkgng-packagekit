#! /usr/bin/awk -f 
BEGIN {
	print "/* Auto-generated, do not edit directly.  Edit 'groups' instead */"
	print "#include \"group_map.h\""
	print "const struct group_mapping group_mappings[] = {"
	COUNT = 0;
}
{
	sub("#.*$", "", $0);
	if (length($0) > 0) {
		print "\t{\"" $1 "\", " $2 "},";
		COUNT++;
	}
}
END {
	print "};";
	print "const size_t num_group_mappings = " COUNT ";";
}
