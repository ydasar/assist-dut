#!/bin/sh

TGE_LV_CAN_0010()
{
    echo "We are at executing ..."
    cd /opt/ltp ; export LTPROOT=$(pwd) ; export PATH=$PATH:$(pwd)/testcases/bin ; cd testcases/bin ; can_test.sh can0 setup ; candump can0
}

TGE_LV_CAN_0016()
{
    cd /opt/ltp ; export LTPROOT=$(pwd) ; export PATH=$PATH:$(pwd)/testcases/bin ; cd testcases/bin ; can_test.sh can0 setup ; candump can0 ; 
}

TGE_LV_CAN_0017()
{
    cd /opt/ltp ; export LTPROOT=$(pwd) ; export PATH=$PATH:$(pwd)/testcases/bin ; cd testcases/bin ; ip link set can0 up type can bitrate 125000 ; ifconfig can0 up ; candump can0
    # cd /opt/ltp ; export LTPROOT=$(pwd) ; export PATH=$PATH:$(pwd)/testcases/bin ; cd testcases/bin ; ip link set can0 up type can bitrate 125000 ; ifconfig can0 up ; canecho can0 -v
}

TGE_LV_CAN_0018()
{
    cd /opt/ltp ; export LTPROOT=$(pwd) ; export PATH=$PATH:$(pwd)/testcases/bin ; cd testcases/bin ; can_test.sh can0 setup ; candump can0 ; 
}

TGE_LV_CAN_0019()
{
    sleep 10
    cd /opt/ltp ; export LTPROOT=$(pwd) ; export PATH=$PATH:$(pwd)/testcases/bin ; cd testcases/bin ; can_test.sh can0 setup ; cangen -I i -L 8 -D 1122334455667788 can0 -v; 
}

TGE_LV_CAN_0021()
{
    cd /opt/ltp ; export LTPROOT=$(pwd) ; export PATH=$PATH:$(pwd)/testcases/bin ; cd testcases/bin ; can_test.sh can0 setup ; i=0 ; while [ $i -lt 10 ] ; do cansend can0 123#R ; sleep 1 ; i=`expr $i + 1` ; done ;
}

TGE_LV_CAN_0022()
{
    cd /opt/ltp ; export LTPROOT=$(pwd) ; export PATH=$PATH:$(pwd)/testcases/bin ; cd testcases/bin ; can_test.sh can0 setup ; cangen -g 400 -e -I i -L 8 -D i can0 -v ; 
}

TGE_LV_CAN_0023()
{
    cd /opt/ltp ; export LTPROOT=$(pwd) ; export PATH=$PATH:$(pwd)/testcases/bin ; cd testcases/bin ; can_test.sh can0 setup ; candump can0 ; 
}



RC=1      
if [ $# -ne 1 ]
then
    usage
fi
    
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
