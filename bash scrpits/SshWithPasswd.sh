#!/usr/bin/expect -f
spawn ssh username@host
expect "password:"
send "password\r"
interact
