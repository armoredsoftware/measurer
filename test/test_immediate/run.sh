echo "TEST: Launching measurer."
PORT=$1
if [ -z "$1" ]
then
    PORT=3000
fi
echo $PORT
../../gdb-7.9/gdb/gdb --port=$PORT > measurer_out &
GDB_PID=$!
echo "TEST: Launching application."
screen -d -m ./app.o > app_out &
while [ "$APP_PID" = "" ]
do
    APP_PID=`pidof app.o`
done
echo "TEST: Launching attester."
./driver.o $PORT $APP_PID > driver_out #&
echo "TEST: Driver terminated"
echo "TEST: Killing Application and Measurer"
kill -9 $APP_PID
kill -9 $GDB_PID
echo "TEST: Dumping Results"
cat test_out
