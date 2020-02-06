#!/bin/sh

TGE_LV_CAN_0010()
{
    /home/root/dut-client "/opt/ltp/testcases/bin/can.sh TGE-LV-CAN-0010 &"
    sleep 5
    cd /opt/ltp ; export LTPROOT=/opt/ltp ; export PATH=$PATH:/opt/ltp/testcases/bin ; cd /opt/ltp/testcases/bin ; can_test.sh can0 1
    return $?
}

TGE_LV_CAN_0016()
{
    /home/root/dut-client "/opt/ltp/testcases/bin/can.sh TGE-LV-CAN-0016 &"
    cd /opt/ltp ; export LTPROOT=/opt/ltp ; export PATH=$PATH:/opt/ltp/testcases/bin ; cd /opt/ltp/testcases/bin ; can_test.sh can0 2
    return $?
}


TGE_LV_CAN_0017()
{
    /home/root/dut-client "/opt/ltp/testcases/bin/can.sh TGE-LV-CAN-0017 &"
    cd /opt/ltp ; export LTPROOT=/opt/ltp ; export PATH=$PATH:/opt/ltp/testcases/bin ; cd /opt/ltp/testcases/bin ; can_test.sh can0 1
    return $?
}


TGE_LV_CAN_0018()
{
    /home/root/dut-client "/opt/ltp/testcases/bin/can.sh TGE-LV-CAN-0018 &"
    cd /opt/ltp ; export LTPROOT=/opt/ltp ; export PATH=$PATH:/opt/ltp/testcases/bin ; cd /opt/ltp/testcases/bin ; can_test.sh can0 7
    return $?
}

TGE_LV_CAN_0019()
{
    /home/root/dut-client "/opt/ltp/testcases/bin/can.sh TGE-LV-CAN-0019 &"
    cd /opt/ltp ; export LTPROOT=/opt/ltp ; export PATH=$PATH:/opt/ltp/testcases/bin ; cd /opt/ltp/testcases/bin ; can_test.sh can0 8
    return $?
}

TGE_LV_CAN_0021()
{
    /home/root/dut-client "/opt/ltp/testcases/bin/can.sh TGE-LV-CAN-0021 &"
    cd /opt/ltp ; export LTPROOT=/opt/ltp ; export PATH=$PATH:/opt/ltp/testcases/bin ; cd /opt/ltp/testcases/bin ; can_test.sh can0 9
    return $?
}

TGE_LV_CAN_0022()
{
    /home/root/dut-client "/opt/ltp/testcases/bin/can.sh TGE-LV-CAN-0022 &"
    cd /opt/ltp ; export LTPROOT=/opt/ltp ; export PATH=$PATH:/opt/ltp/testcases/bin ; cd /opt/ltp/testcases/bin ; can_test.sh can0 10
    return $?
}

TGE_LV_CAN_0023()
{
    /home/root/dut-client "/opt/ltp/testcases/bin/can.sh TGE-LV-CAN-0023 &"
    cd /opt/ltp ; export LTPROOT=/opt/ltp ; export PATH=$PATH:/opt/ltp/testcases/bin ; cd /opt/ltp/testcases/bin ; can_test.sh can0 11
    return $?
}


usage()                                                                                            
{
    echo "can_test.sh [TEST ID]"
    exit 1
}                     



RC=1                                                 
if [ $# -ne 1 ]                                                                                    
then                           
    usage                                             
fi                                                                             

output=`/home/root/dut-client "ConsoleLogsClear"`
echo "output is $output"
if [[ $output == *"fail"* ]]
then
    exit -1
fi

/home/root/dut-client "pkill candump &"
/home/root/dut-client "pkill cangen &"
                                                                                                    
case "$1" in                                                                                        
    TGE-LV-CAN-0010)                                                                   
        TGE_LV_CAN_0010 || exit $RC                                                  
        ;;                                                                    
    TGE-LV-CAN-0016)                                                                                         
        TGE_LV_CAN_0016 || exit $RC
        ;;          
    TGE-LV-CAN-0017)                                                                                         
        TGE_LV_CAN_0017 || exit $RC
        ;;          
    TGE-LV-CAN-0018)                                                                                         
        TGE_LV_CAN_0018 || exit $RC
        ;;          
    TGE-LV-CAN-0019)                                                                                         
        TGE_LV_CAN_0019 || exit $RC
        ;;          
    TGE-LV-CAN-0021)                                                                                         
        TGE_LV_CAN_0021 || exit $RC
        ;;          
    TGE-LV-CAN-0022)                                                                                         
        TGE_LV_CAN_0022 || exit $RC
        ;;          
    TGE-LV-CAN-0023)                                                                                         
        TGE_LV_CAN_0023 || exit $RC
        ;;          
    *)                                                                   
        usage                                                                    
        ;;                                                                    
esac 

/home/root/dut-client "pkill candump &"
/home/root/dut-client "pkill cangen &"

output=`/home/root/dut-client "ConsoleLogsClear"`
if [[ $output == *"fail"* ]]
then
    exit -1
fi
