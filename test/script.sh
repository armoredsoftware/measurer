echo "TEST: Launching measurer."
../gdb-7.9/gdb/gdb > measurer_out &
GDB_PID=$!
echo "TEST: Launching application."
screen -d -m ./app1.o > app_out &
while [ "$APP_PID" = "" ]
do
    APP_PID=`pidof app1.o`
done
#./app1.o &
#APP_PID=$!
#wait
#APP_PID=$1
echo "TEST: Launching attester."
#./app1.o &
./driver1.o $APP_PID > driver_out #&
#../driver/driver1.o &
echo "TEST: Driver terminated"
echo "TEST: Killing Application and Measurer"
kill -9 $APP_PID
kill -9 $GDB_PID
echo "TEST: Dumping Results"
cat test_out
