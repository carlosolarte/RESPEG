maude c89 | grep "State\|erewrite" > /tmp/pal.txt &
maude c89-cut | grep "State\|erewrite" > /tmp/pal-cut.txt 
wait
vim -d /tmp/pal.txt /tmp/pal-cut.txt
