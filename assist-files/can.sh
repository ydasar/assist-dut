#!/bin/sh

TGE-LV-CAN-0010()
{
    echo "We are at executing ..."
    cd /opt/ltp ; export LTPROOT=$(pwd) ; export PATH=$PATH:$(pwd)/testcases/bin ; cd testcases/bin ; can_test.sh can0 setup ; candump can0
}

TGE-LV-CAN-0016()
{
    cd /opt/ltp ; export LTPROOT=$(pwd) ; export PATH=$PATH:$(pwd)/testcases/bin ; cd testcases/bin ; can_test.sh can0 setup ; candump can0 ; 
}

TGE-LV-CAN-0017()
{
    cd /opt/ltp ; export LTPROOT=$(pwd) ; export PATH=$PATH:$(pwd)/testcases/bin ; cd testcases/bin ; ip link set can0 up type can bitrate 125000 ; ifconfig can0 up ; candump can0
    # cd /opt/ltp ; export LTPROOT=$(pwd) ; export PATH=$PATH:$(pwd)/testcases/bin ; cd testcases/bin ; ip link set can0 up type can bitrate 125000 ; ifconfig can0 up ; canecho can0 -v
}

TGE-LV-CAN-0018()
{
    cd /opt/ltp ; export LTPROOT=$(pwd) ; export PATH=$PATH:$(pwd)/testcases/bin ; cd testcases/bin ; can_test.sh can0 setup ; candump can0 ; 
}

TGE-LV-CAN-0019()
{
    sleep 10
    cd /opt/ltp ; export LTPROOT=$(pwd) ; export PATH=$PATH:$(pwd)/testcases/bin ; cd testcases/bin ; can_test.sh can0 setup ; cangen -I i -L 8 -D 1122334455667788 can0 -v; 
}

TGE-LV-CAN-0021()
{
    cd /opt/ltp ; export LTPROOT=$(pwd) ; export PATH=$PATH:$(pwd)/testcases/bin ; cd testcases/bin ; can_test.sh can0 setup ; i=0 ; while [ $i -lt 10 ] ; do cansend can0 123#R ; sleep 1 ; i=`expr $i + 1` ; done ;
}

TGE-LV-CAN-0022()
{
    cd /opt/ltp ; export LTPROOT=$(pwd) ; export PATH=$PATH:$(pwd)/testcases/bin ; cd testcases/bin ; can_test.sh can0 setup ; cangen -g 400 -e -I i -L 8 -D i can0 -v ; 
}

TGE-LV-CAN-0023()
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
        TGE-LV-CAN-0010 || exit $RC
        ;;
    TGE-LV-CAN-0016)
        TGE-LV-CAN-0016 || exit $RC
        ;;          
    TGE-LV-CAN-0017)
        TGE-LV-CAN-0017 || exit $RC
        ;;          
    TGE-LV-CAN-0018)
        TGE-LV-CAN-0018 || exit $RC
        ;;          
    TGE-LV-CAN-0019)
        TGE-LV-CAN-0019 || exit $RC
        ;;          
    TGE-LV-CAN-0021)
        TGE-LV-CAN-0021 || exit $RC
        ;;          
    TGE-LV-CAN-0022)
        TGE-LV-CAN-0022 || exit $RC
        ;;          
    TGE-LV-CAN-0023)
        TGE-LV-CAN-0023 || exit $RC
        ;;          
    *)                         
        usage
        ;;          
esac
