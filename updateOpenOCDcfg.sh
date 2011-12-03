# update openocd.cfg across the development directory
# requires find to be installed

find ./ -name 'openocd.cfg' -exec cp ./openocd.cfg '{}' \;

