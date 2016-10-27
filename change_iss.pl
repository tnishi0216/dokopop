#!perl
# Change the string in the .iss file.

sub usage
{
print << "EOT";
Usage:
  change_iss.pl [input filename] [section name] [key name] [substituted value]
EOT
}

$u_input = $ARGV[0];
$u_section = $ARGV[1];
$u_key = $ARGV[2];
$u_value = $ARGV[3];

if (!$u_value){
	usage();
	exit(1);
}

open F,$u_input or die "open error $u_input";

while (<F>){
	if (length>0){
		if (/^\[(.+)\]/){
			# Section start
			$section = $1;
			$key = '';
		} else {
			if (/^(.+?)=/){
				# Found a key.
				$key = $1;
				if ($u_section =~ /^$section$/i){
					# found the same section
					if ($u_key =~ /^$key$/i){
						$_ = "$key=$u_value\n";
					}
				}
			}
		}
	}
	print $_;
}


