#!/bin/bash
#pptpd流量监控
#zhaoyun 2012-06-14
while true
do
        #判断有几个登陆的接口
        row=$(ip add |grep global |grep ppp |wc -l)
	echo " $row "
        for ((i=1;i<=$row;i++))
        do
                PPPX=$(ip add |grep global |grep ppp |awk '{print $4,$7}' |awk '{print $2}'|head -n$i |tail -1 )
                ip=`ip add |grep global |grep ppp |awk '{print $4,$7}' |awk -F/ '{print $1" "$2}' |awk '{print $1}' |head -n$i |tail -1`
                FLOWA=/tmp/.flow.$PPPX.a
                RESULT=`ifconfig $PPPX |grep "RX byte" |awk '{print $2"    "$6}' |awk -Fbytes: '{print "INPUT  "$2"OUTPUT  "$3}' `
                echo " $RESULT ">$FLOWA
        done
        sleep 1
        for ((i1=1;i1<=$row;i1++))
        do
                PPPX=$(ip add |grep global |grep ppp |awk '{print $4,$7}' |awk '{print $2}'|head -n$i |tail -1 )
                ip=`ip add |grep global |grep ppp |awk '{print $4,$7}' |awk -F/ '{print $1" "$2}' |awk '{print $1}' |head -n$i |tail -1`
                FLOWB=/tmp/.flow.$PPPX.b
                RESULT=`ifconfig $PPPX |grep "RX byte" |awk '{print $2"    "$6}' |awk -Fbytes: '{print "INPUT  "$2"OUTPUT  "$3}' `
                echo "$RESULT ">$FLOWB
                INPUTB=`awk '{print $2}' $FLOWB`
                OUTPUTB=`awk '{print $4}' $FLOWB`
                INPUTA=`awk '{print $2}' $FLOWA`
                OUTPUTA=`awk '{print $4}' $FLOWA`
                INPUTKB=$(($INPUTB-$INPUTA))
                OUTPUTKB=$(($OUTPUTB-$OUTPUTA))
                a=$(echo $INPUTKB  |grep '-' &>/dev/null; echo $?)
                b=$(echo $OUTPUTKB |grep '-' &>/dev/null; echo $?)
                if [ ! $a -eq 0 ] && [ ! $b -eq 0 ] ; then
                   if [ ! $INPUTKB -eq 0  ] && [ ! $OUTPUTKB -eq 0 ] ; then
                      echo "$ip $PPPX $INPUTKB $OUTPUTKB" >> Flow.log
                   fi
                fi
              unset INPUTKB OUTPUTKB OUTPUTB INPUTB INPUTA OUTPUTA
        done
done
