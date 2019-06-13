welcome_strings=(\
"你又在绘制新航线？🤨"
"现在你要做什么？🤤"
"你变成了一具木乃伊!😨"
"劲爆大象部落🐘"
"不要慌，问题不大🤣"
"留下了没技术的泪水😢"
)
random_string=${welcome_strings[$RANDOM % ${#welcome_strings[@]} ]}
cow_flags=("" "-d" "-g" "-p" "-s" "-t" "-w")
random_flag=${cow_flags[$RANDOM % ${#cow_flags[@]} ]}

stty -echo
cowsay ${random_flag} "${random_string}"|lolcat -F 0.5
stty echo
