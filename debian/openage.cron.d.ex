#
# Regular cron jobs for the openage package
#
0 4	* * *	root	[ -x /usr/bin/openage_maintenance ] && /usr/bin/openage_maintenance
