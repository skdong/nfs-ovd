msg=`ps -ef | grep $1 | grep -v grep `
while (( $?!=0 ))
	do
	msg=`ps -ef | grep $1 | grep -v grep `
	done
pid=`echo $msg | awk '{print $2}'`
mgs="gdb /usr/sbin/xrdp-sessvc "$pid
echo $mgs
$mgs
