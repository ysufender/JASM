set -e

run=false

while test $# -gt 0; do
    case "$1" in
        -r|--run)
            run=true
            shift
            ;;
        *)
            shift
            break
            ;;
    esac
done

if [ $run == false ]; then
    ./build.sh
    echo
fi
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
