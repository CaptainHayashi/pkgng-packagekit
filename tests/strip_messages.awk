# Strips in-band auxiliary messages from a `pkcon` run, leaving only the results
BEGIN { in_description=0 }
!/Message:.*/ && in_description { print }
/Results:/ { in_description=1 }
