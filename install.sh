if command -v doas &> /dev/null; then
    PRIVILEGE="doas"
elif command -v sudo &> /dev/null; then
    PRIVILEGE="sudo"
else
    echo "Neither 'doas' nor 'sudo' found. T-T"
    exit 1
fi

gcc -o cmixer main.c -lpulse
$PRIVILEGE cp -r cmixer /usr/local/bin/
