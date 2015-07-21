#echo "Launching application."
#./app1.o &
#APP_PID=$!
#disown
APP_PID=$1
echo "PID="
echo $APP_PID
echo "Launching driver."
./driver.o $APP_PID #> driver_out
kill -9 $APP_PID
