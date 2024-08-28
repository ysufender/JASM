set -e

./build.sh
echo
echo "-----------------------------------------"
echo "                RUNNING                  "
echo "-----------------------------------------"
echo
build/bin/Debug/jasm -w asm -S -I test.jasm
echo
echo "-----------------------------------------"
echo "                OUTPUT                   "
echo "-----------------------------------------"
echo
hexdump asm/test.jasm.jo
