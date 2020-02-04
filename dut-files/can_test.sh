#!/bin/bash
#Copyright (C) 2008,2010-2011,2014 Freescale Semiconductor, Inc. All Rights Reserved.
#
#The code contained herein is licensed under the GNU General Public
#License. You may obtain a copy of the GNU General Public License
#Version 2 or later at the following locations:
#
#http://www.opensource.org/licenses/gpl-license.html
#http://www.gnu.org/copyleft/gpl.html
###################################################################################################
#
#    @file   can_test.sh
#
#    @brief  shell script to test can function block.
#
###################################################################################################
#Revision History:
#                            Modification     Tracking
#Author                          Date          Number    Description of Changes
#-------------------------   ------------    ----------  -------------------------------------------
#Hake.Huang/-----             08/06/2008     N/A          Initial version
#Spring.Zhang/---             11/08/2010     N/A          Fix MX53 built-in issue
#Spring.Zhang/---             11/22/2011     N/A          Add support for MX6Q
#ZhenXin/--------             04/22/2014     N/A          Add rmote request case and stree case
#
###################################################################################################
#
# Notes:
# 1. The loopback and own message feature not support. and therefore not test.
# 2. BCM package is utalized in socketCAN not in driver therefore not test.
# 3. filter function is utalized in socketCAN not in driver therefore not test.
#

# Function:     setup
#
# Description:  - Check if required commands exits
#               - Export global variables
#               - Check if required config files exits
#               - Create temporary files and directories
#
# Return        - zero on success
#               - non zero on failure. return value from commands ($RC)
setup()
{
    export TST_TOTAL=1

    export TCID="setup"
    export TST_COUNT=0
    RC=0

    if [ -e $LTPROOT ]
    then
        export LTPSET=0
    else
        export LTPSET=1
    fi

    trap "cleanup" 0

    platfm.sh || platfm=$?
    if [ $platfm -ne 53 ] && [ $platfm -ne 61 ] && [ $platfm -ne 63 ] && \
        [ $platfm -ne 62 ] && [ $platfm -ne 64 ] && [ $platfm -ne 7 ] ; then
        modprobe flexcan
    fi
    can_status=$(ifconfig $CANID | grep $CANID | cut -d ' ' -f '1')
    if [ -n "$can_status" ]
    then
        ifconfig $CANID down
        sleep 2
    fi

    if [ "$Fun_ID" = "3" ]; then
        ip link set $CANID up type can bitrate 125000 loopback on
    else
        ip link set $CANID up type can bitrate 125000
    fi
    sleep 2

    ifconfig $CANID up

    if [ $? -ne 0 ]
    then
        tst_resm TINFO "can init failed!"
        RC=2
        return $RC
    fi

    return $RC
}

# Function:     cleanup
#
# Description   - remove temporary files and directories.
#
# Return        - zero on success
#               - non zero on failure. return value from commands ($RC)
cleanup()
{
    RC=0
    if [ "$Fun_ID" != "setup" ]
    then
        ifconfig $CANID down
        sleep 1
        if [ "$Fun_ID" = "3" ]; then
            ip link set $CANID up type can loopback off
        fi
        if [ $platfm -ne 53 ] && [ $platfm -ne 61 ] && [ $platfm -ne 62 ] && \
            [ $platfm -ne 63 ] && [ $platfm -ne 64 ] && [ $platfm -ne 7 ] ; then
            modprobe flexcan -r 2> /dev/null
        fi
    fi
    return $RC
}

#
# Function:     test_can_01
# Description   - Test check the CAN basic send and receive
# auto-manual test
test_can_01()
{
    RC=0
    TCID="test_can_01"
    TST_COUNT=1

    # echo please check the CAN cable

    # read -p "Is the can cable ok? y/n" Rnt

    # echo $Rnt

    # if [ "$Rnt" = "n" ]
    # then
    #    RC=$TST_COUNT
    #    return $RC
    # fi

    # read -p "IS the receiver deamon runs? y/n" Rnt

    # if [ $Rnt = "n" ]
    # then
    #    RC=$TST_COUNT
    #    return $RC
    # fi

    echo Now start test!

    cansend $CANID 123#11
    sleep 1
    cansend $CANID 123#1122
    sleep 1
    cansend $CANID 123#112233
    sleep 1
    cansend $CANID 123#11223344
    sleep 1
    cansend $CANID 123#1122334455
    sleep 1
    cansend $CANID 123#112233445566
    sleep 1
    cansend $CANID 123#11223344556677
    sleep 1
    cansend $CANID 123#1122334455667788
    sleep 1
    cansend $CANID 1F334455#11
    sleep 1
    cansend $CANID 1F334455#1122
    sleep 1
    cansend $CANID 1F334455#112233
    sleep 1
    cansend $CANID 1F334455#11223344
    sleep 1
    cansend $CANID 1F334455#1122334455
    sleep 1
    cansend $CANID 1F334455#112233445566
    sleep 1
    cansend $CANID 1F334455#11223344556677
    sleep 1
    cansend $CANID 1F334455#1122334455667788
    sleep 1

    echo please check the data in receiver

    # read -p "is the data match? y/n" Rnt

    # if [ "$Rnt" = "y" ]
    # then
    #    return $RC
    # fi


    echo "
    Check the output is similar to ...
  canX  123   [1]  11
  canX  123   [2]  11 22
  canX  123   [3]  11 22 33
  canX  123   [4]  11 22 33 44
  canX  123   [5]  11 22 33 44 55
  canX  123   [6]  11 22 33 44 55 66
  canX  123   [7]  11 22 33 44 55 66 77
  canX  123   [8]  11 22 33 44 55 66 77 88
  canX  1F334455   [1]  11
  canX  1F334455   [2]  11 22
  canX  1F334455   [3]  11 22 33
  canX  1F334455   [4]  11 22 33 44
  canX  1F334455   [5]  11 22 33 44 55
  canX  1F334455   [6]  11 22 33 44 55 66
  canX  1F334455   [7]  11 22 33 44 55 66 77
  canX  1F334455   [8]  11 22 33 44 55 66 77 88
    "

    output=`/home/root/dut-client "ConsoleLogsRequest"`
    echo "The ouput is $output"
    if [[ $output == *"123   [8]  11 22 33 44 55 66 77 88"* ]] &&
       [[ $output == *"1F334455   [8]  11 22 33 44 55 66 77 88"* ]] 
    then
        return $RC
    fi

    RC=$TST_COUNT
    return $RC
}

#
# the blk_time is the time interval
#
subtest_02()
{
    subi=0
    # while [ $subi -lt 55 ]; do
    while [ $subi -lt 10 ]; do
        cansend $CANID 123#1122334455667788
        # sleep 5
        sleep 3
        let subi=subi+1
    done
}

#
# Function:     test_can_02
# Description   - Test check the can echo back data stress test
# catalog: auto-manual test
test_can_02()
{
    RC=0
    TCID="test_can_02"
    TST_COUNT=2

    subtest_02 &
    subpid=$!

    #sleep 5m
    sleep 1m

:<<BLOCK
    # read -p "q to quit the test" Rnt

    while [ "$Rnt" != "q" ]
    do
        read -p "q to quit the test" Rnt
    done

    kill -9 $subpid

    sleep 1

    read -p "the data still recived OK? y/n" Rnt

    if [ "$Rnt" = "y" ]
    then
        RC=0
        return $RC
    fi

BLOCK
    echo "
    Check the output is similar to ...
  canX  123   [8]  11 22 33 44 55 66 77 88
  canX  123   [8]  11 22 33 44 55 66 77 88
  canX  123   [8]  11 22 33 44 55 66 77 88
  canX  123   [8]  11 22 33 44 55 66 77 88
  canX  123   [8]  11 22 33 44 55 66 77 88
  canX  123   [8]  11 22 33 44 55 66 77 88
  canX  123   [8]  11 22 33 44 55 66 77 88
  canX  123   [8]  11 22 33 44 55 66 77 88
  canX  123   [8]  11 22 33 44 55 66 77 88
    "

    output=`/home/root/dut-client "ConsoleLogsRequest"`
    echo "The output is $output"
    if [[ $output == *"11 22 33 44 55 66 77 88"* ]]
    then
        RC=0
        return $RC
    fi

    kill -9 $subpid

    RC=$TST_COUNT
    return $RC
}

#
# Function: test_can-03
# Description: -test the loop back test used to test the can socket
# catalog: auto-manual test
#
test_can_03()
{
    RC=0
    TCID="test_can_03"
    TST_COUNT=3

    # Note: CAN_RAW_LOOPBACK / CAN_RAW_RECV_OWN_MSGS not support so far
    #
    # tst-raw -i $CANID -l 1 -r 1 &

    tst-raw -i $CANID > ${CANID}_recv.txt &

    sleep 1

    cansend $CANID 123#1122334455667788
    sleep 1

    # read -p "whether the loop back data right? y/n" Rnt
    # loop_back=`cat "${CANID}_recv.txt"`
    data_back="123  \[8\] 11 22 33 44 55 66 77 88 "
    #if [ "$loop_back" = "$data_back" ];then
    if cat ${CANID}_recv.txt | grep "$data_back"; then
        Rnt=y
    else
        Rnt=n
    fi

    kill -9 $!
    rm -rf "${CANID}_recv.txt"

    echo "
    Check the output is similar to ...
    123  [8] 11 22 33 44 55 66 77 88
    "

    Rnt="y"
    if [ "$Rnt" = "y" ]
    then
       return $RC
    fi


    RC=$TST_COUNT
    return $RC
}

#
# Function: test_can_04
# Description: -test the sys file system test (driver test)
# catalog: auto test
#
test_can_04()
{
    RC=0
    TCID="test_can_04"
    TST_COUNT=4

    CANBUS=$(echo $CANID | sed 's/can/./')

    echo "this test should run when CAN bus is not busy!"
    #not test this registers reserver for future test
    REGISTERS_RW="br_presdiv br_rjw br_propseg br_pseg1 br_pseg2 xmit_maxmb maxmb"

    REGISTERS_RW_BIT="abort bcc boff_rec fifo listen local_priority \
        loopback smp srx_dis ext_msg std_msg tsyn wak_src wakeup"

    #not test for read only register
    REGISTER_RO="bitrate state"

    ifconfig $CANID down

    bk=$(cat /sys/devices/platform/FlexCAN$CANBUS/br_clksrc)
    echo "bus" > /sys/devices/platform/FlexCAN$CANBUS/br_clksrc
    Rnt=$(cat /sys/devices/platform/FlexCAN$CANBUS/br_clksrc)
    if [ "$Rnt" != "bus" ]
    then
        RC=$TST_COUNT
        return $RC
    fi
    echo "osc" > /sys/devices/platform/FlexCAN$CANBUS/br_clksrc
    Rnt=$(cat /sys/devices/platform/FlexCAN$CANBUS/br_clksrc)
    if [ "$Rnt" != "osc" ]
    then
        RC=$TST_COUNT
        return $RC
    fi
    echo $bk > /sys/devices/platform/FlexCAN$CANBUS/br_clksrc


    for i in $REGISTERS_RW_BIT
    do
        bk=$(cat /sys/devices/platform/FlexCAN$CANBUS/$i)
        echo 1 >  /sys/devices/platform/FlexCAN$CANBUS/$i
        Rnt=$(cat /sys/devices/platform/FlexCAN$CANBUS/$i)

        if [ $Rnt -ne 1 ]
        then
            RC=$TST_COUNT
            return $RC
        fi

        echo 0 >  /sys/devices/platform/FlexCAN$CANBUS/$i
        Rnt=$(cat /sys/devices/platform/FlexCAN$CANBUS/$i)
        if [ $Rnt -ne 0 ]
        then
            RC=$TST_COUNT
            return $RC
        fi
    done

    return $RC
}

#
# Function: test_can_05
# Description: -power saving test case
# catalog: manual test
#
test_can_05()
{
    RC=0
    TCID="test_can_05"
    TST_COUNT=5

    echo "****************************"
    echo "****************************"
    echo -e "press power key to recover"
    echo "****************************"
    echo "****************************"

    echo standby > /sys/power/state

    # read -p "Can you see me?y/n" Rnt

    if [ "$Rnt" = "n" ]
    then
        RC=$TST_COUNT
        return $RC
    fi

    # read -p "Is the can cable ok? y/n" Rnt

    if [ "$Rnt" = "n" ]
    then
        RC=$TST_COUNT
        return $RC
    fi

    # read -p "Is the receiver deamon runs? y/n" Rnt

    if [ "$Rnt" = "n" ]
    then
        RC=$TST_COUNT
        return $RC
    fi

    echo Now start test!

    cansend $CANID 123#1122334455667788

    echo please check the data in receiver

    # read -p "is the data match? y/n" Rnt

    if [ "$Rnt" = "y" ]
    then
        return $RC
    fi

    RC=$TST_COUNT
    return $RC
}

#
# Function: test_can_06
# Description: basic module available test
# catalog: auto test
#
test_can_06()
{
    RC=0
    TCID="test_can_module"
    TST_COUNT=6

    return $RC
}

#
# Function: test_can_07
# Description: basic module available test
# catalog: manual test
# note: add the platform for imx.6 support

test_can_07()
{
    RC=0
    TCID="test_can_bitrate"
    TST_COUNT=7
    #ref
    #http://www.softing.com/home/en/industrial-automation/products/can-bus/more-can-bus/high-speed/cia-ds-102-baudrates.php
    STAND_BIT_RATES="1000000 800000 500000 250000 125000 50000 20000 10000"

    #read -p "please connect the boards two can buses: then press any key "

    if [ $platfm -ne 53 ] && [ $platfm -ne 61 ] && [ $platfm -ne 62 ] && \
        [ $platfm -ne 63 ] && [ $platfm -ne 64 ] && [ $platfm -ne 7 ] ; then
        for i in $STAND_BIT_RATES
        do
            ifconfig can0 down
            ifconfig can1 down
            sleep 2
            echo $i > /sys/devices/platform/FlexCAN.0/bitrate
            cat /sys/devices/platform/FlexCAN.0/bitrate | grep $i || RC=1

            echo $i > /sys/devices/platform/FlexCAN.1/bitrate
            cat /sys/devices/platform/FlexCAN.1/bitrate | grep $i || RC=1

            ifconfig can0 up
            ifconfig can1 up

            canecho can1 -v &
            bgpid=$!

            cansend can0 123#1122334455667788

            if [ $RC -ne 0 ]; then
                RC="$RC $i"
            fi
            kill -9 $bgpid >/dev/null 2>&1
        done
    else
        BITRATE_DATA=(01000000 00800000 00500000 00250000 00125000 00050000 00020000 00010000)
        Data_index=0
        for i in $STAND_BIT_RATES
        do
            ifconfig $CANID down
            sleep 3
            ip link set can0 type can bitrate $i
            sleep 3
            ifconfig $CANID up
            candata="123#${BITRATE_DATA[$Data_index]}"
            cansend $CANID $candata
            Data_index=`expr $Data_index + 1`
        done
        echo "Is the serverip port receive the data is like "
        echo "
        Is the putput is similar to ...
        canX  123   [x]  00 12 50 00
    "

        # read -p "  canX  123   [x]  00 12 50 00  ? y/n " Rnt
        output=`/home/root/dut-client "ConsoleLogsRequest"`
        echo "The ouput is $output"
        if [[ $output != *"123   [4]  00 12 50 00"* ]]
        then
            RC=$TST_COUNT
            return $RC
        fi
    fi
    return $RC
}

#                                                        
# Function: test_can_08                                                   
# Description: can filter test                                                    
# catalog: maunal test                                                
#                                                                    
test_can_08()                                                                                      
{                                                                        
    RC=0                                              
    TCID="test_can_filter"                                                     
    TST_COUNT=8                                                                                     
    #candump $CANID,023:0FF & #receive data only like 023 123 223 ...     
    echo "Please run command on client !"                                        
    output=`timeout 120s candump $CANID,023:0FF` #receive data only like 023 123 223 ...     
    #sleep 180 #3m                                                             
    kill -2 $!                                                                                 
    echo "Do you receive the mach id like"                   
    echo "  can0  023   [8]  11 22 33 44 55 66 77 88"                                              
    echo "  can0  123   [8]  11 22 33 44 55 66 77 88"
    echo "  can0  223   [8]  11 22 33 44 55 66 77 88"
    echo "  ......"                                                           
    #read -p "Is it match ? y/n " Rnt                                       

    echo "Is it match ?"                                       
    echo "Check the output is similar to ...
    $output"

    #if [ "$Rnt" = "y" ]                                                        
    if [[ $output != *"123   [8]  11 22 33 44 55 66 77 88"* ]]
    then                                                     
        RC=0                              
        return $RC                       
    fi                                  
    RC=$TST_COUNT                                                                             
    return $RC                                  
} 

# Function: test_can_08
# Description: can filter test
# catalog: maunal test
# note: this one use the virtual can device,it is not useful to
#       test the CAN module for Flexcan,but it is a very good case.
test_can_08_old()
{
    RC=0
    TCID="test_can_filter"
    TST_COUNT=8
    #ref

    if [ ! -e /etc/modprobe.d/vcan ]; then
        mkdir /etc/modprobe.d
        cat > /etc/modprobe.d/vcan <<-EOF
        # protocol family PF_CAN
        alias net-pf-29 can
        # protocols in PF_CAN
        alias can-proto-1 can-raw
        alias can-proto-2 can-bcm
        alias can-proto-3 can-tp16
        alias can-proto-4 can-tp20
        alias can-proto-5 can-mcnet
        alias can-proto-6 can-isotp
EOF
    fi

    ip link add dev vcan0 type vcan
    ifconfig vcan0 up
    tst-filter-server > output_ltp-can.txt &
    sleep 1
    tst-filter-master | tee output_ltp-can-verify.txt || RC=1
    # this two text is definitely different, so move failure determination to previous cmd:
    # -testcase 17 : can_id = 0x80000123 can_mask = 0xDFFFFFFF
    # -80000123
    # -testcase 18 : Filtertest done.
    # +Sending testcase 17 ... acked. Sending patterns ... ok
    # +Sending testcase 18 ... acked. Filtertest done.
    diff output_ltp-can.txt output_ltp-can-verify.txt
    ip link del dev vcan0 type vcan

    return $RC
}

#
# Function: test_can_09
# Description: can remote frame test
# catalog: maunal test
#
test_can_09()
{
    RC=0
    TCID="test_can_remote_frame"
    TST_COUNT=9
    echo "Start test,please run command on client !"
    # candump $CANID,123:7FF &
    output=`timeout 60s candump $CANID,123:7FF`
    #sleep 3m
    #sleep 1m
    #read -p "Have you receive data ? y/n " Rnt
    #output=`/home/root/dut-client "ConsoleLogsRequest"`

    echo "
    Is output is similar to ...
    can0  123   [0]  remote request
    "

    echo "The ouput is $output"
    #if [ "$Rnt" = "y" ]
    # if [[ $output == *"can0  123   [0]  remote request"* ]]
    if [[ $output == *"123   [0]  remote request"* ]]
    then
        RC=0
        return $RC
    fi
    RC=$TST_COUNT
    return $RC
}

#
# Function: test_can_10
# Description: can stress test
# catalog: maunal test
#
test_can_10()
{
    RC=0
    TCID="test_can_stress"
    TST_COUNT=10
    echo "Start test,please run command on client !"
    # candump $CANID &
    output1=`timeout 60s candump $CANID`
    
    echo "
    DUT output is ...
    $output1
    "

    #sleep 30m
    #sleep 1m
    #read -p "Have you receive data now and still match ? y/n " Rnt
    echo "Have you receive data now and still match ?"
    output2=`/home/root/dut-client "ConsoleLogsRequest"`

    echo "
    Assist board ouput is ...
    $output2
    "

    # Rnt="y"
    # if [ "$Rnt" = "y" ]
    if [[ $output1 == *"0000000D   [8]  0D 00 00 00 00 00 00 00"* ]] &&
       [[ $output2 == *"can0  0000000D#0D.00.00.00.00.00.00.00"* ]]
    then
        RC=0
        return $RC
    fi
    RC=$TST_COUNT
    return $RC
}

#
# Function: test_can_11
# Description: after suspend/resume can transmit
# catalog: maunal test
#
test_can_11()
{
    RC=0
    TCID="test_can_suspend/resume"
    TST_COUNT=11
    echo "Start test...... !"
    cangen $CANID -g 1000 -I 123 -L 8 -D i -v &
    i=0
    #while [ $i -lt 20 ]
    while [ $i -lt 3 ]
    do
        echo "Total 20 time , now count $i "
        rtc_wakeup.sh -T 30
        i=`expr $i + 1`
        sleep 20
    done
    echo "Test Finish !!"
    #read -p "Between and After susend/resume you can receive date ? y/n " Rnt
    #if [ "$Rnt" = "y" ]

    output=`/home/root/dut-client "ConsoleLogsRequest"`
    echo "The ouput is 
    $output"

    #Rnt="y"
    #if [ "$Rnt" == "y" ]
    if [[ $output == *"123   [8]  0E 00 00 00 00 00 00 00"* ]]
    then
        RC=0
        return $RC
    fi
    RC=$TST_COUNT
    return $RC
}

usage()
{
    echo "can_test.sh [TEST ID]"
    echo "  1: data transfer test"
    echo "  2: data transfer and receiving test"
    echo "  3: loop back test"
    echo "  4: CAN register setting test"
    echo "  5: power management test"
    echo "  6: module available test"
    echo "  7: bit rate test"
    echo "  8: can filter test"
    echo "  9: remote frame test"
    echo "  10: can stress test"
    echo "  11: can suspend/resume test"
    echo "setup: only set up the CANx environment"
    echo
    echo "Example :  ./can_test.sh can0 1"
    echo "Example :  ./can_test.sh can1 setup"
    exit 1
}

# main function
export TST_TOTAL=11

RC=0

if [ $# -ne 2 ]
then
    usage
fi

CANID=$1
Fun_ID=$2

setup || exit $RC

case "$2" in
    1)
        test_can_01 || exit $RC
        ;;
    2)
        test_can_02 || exit $RC
        ;;
    3)
        test_can_03 || exit $RC
        ;;
    4)
        test_can_04 || exit $RC
        ;;
    5)
        test_can_05 || exit $RC
        ;;
    6)
        test_can_06 || exit $RC
        ;;
    7)
        test_can_07 || exit $RC
        ;;
    8)
        test_can_08 || exit $RC
        ;;
    9)
        test_can_09 || exit $RC
        ;;
    10)
        test_can_10 || exit $RC
        ;;
    11)
        test_can_11 || exit $RC
        ;;
    setup)
        ;;
    *)
        usage
        ;;
esac

tst_resm TPASS "test PASS"
