echo "TEST: Launching measurer."
PORT=$1
if [ -z "$1" ]
then
    PORT=3000
fi
echo $PORT
../../msrrd/gdb/msrrd --port=$PORT > measurer.out &
GDB_PID=$!
echo "TEST: Launching application."
screen -d -m env LD_PRELOAD=`pwd`/../../prestop/prestop.so ./app.o > app.out &
while [ "$APP_PID" = "" ]
do
    APP_PID=`pidof app.o`
done
echo "TEST: Launching attester."
./driver.o $PORT $APP_PID > driver.out #&
echo "TEST: Driver terminated"
echo "TEST: Killing Application and Measurer"
kill -9 $APP_PID
kill -9 $GDB_PID
echo "TEST: Dumping Results"
cat test.out
