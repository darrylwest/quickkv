#!/usr/bin/env bash
#
# dpw@plaza.localdomain | 2024-12-13 03:28:43
#
#

set -eu

export root=`pwd`

export project=quickkv
export port=9696

export os="$(uname -s)"
export arch="$(uname -m)"

[ $os == "Linux" ] && {
    # export CC=/usr/bin/clang
    # export CXX=/usr/bin/clang++
    export CC=/usr/local/bin/gcc
    export CXX=/usr/local/bin/g++
    export FLAGS="-j8"
} || {
    export FLAGS="-j20"
}

# parse the cli
while [[ $# -gt 0 ]]
do
    case $1 in
        init)
            [ -d build/_deps/ ] && {
                mv build/_deps/ . && /bin/rm -fr build && mkdir build && mv _deps/ build/ 
            } || {
                mkdir build
            }
            (cd build && cmake ..)

            shift
        ;;
        build)
            clear

            (cd build && time make $FLAGS || exit 1)

            shift
        ;;
        unit|test)
            # (cd build && time make $FLAGS)
            $root/build/unit_tests

            shift
        ;;
        run-data)
            (cd build && make $FLAGS)
            export TESTING=true
            $root/build/data-tasks

            shift
        ;;
        format)
            clang-format -i include/quickkv/*.hpp src/*.cpp
            git status -s

            shift
        ;;
        clean)
            (cd build && make clean && /bin/rm -f datalogger-unit)
        
            shift
        ;;
        clo*)
            /bin/rm -fr build/

            shift
        ;;
        multi)
            ./build/examples/multi-store

            shift
        ;;
        watch)
            watchexec -c -w src/ -w include/ -w examples/ -w tests/ -e h,hpp,cpp ./mk build unit multi

            exit 0
        ;;
        run|exam*)
            for f in ./build/examples/*
            do
                ./$f
            done
            
            shift
        ;;

        pull)
            git pull

            shift
        ;;

        help)
            echo "Targets:"
            echo ""
            echo "   init     : run the cmake command to create the build folder"
            echo "   build    : compile cryptor to the build folder"
            echo "   unit     : run the unit tests"
            echo "   format   : runs clang-format over includes and src"
            echo "   watch    : run watcher over source and include"
            echo "   pull     : pull the latest repo changes"
            echo "   clean    : remove binary builds but leave the build folder"
            echo "   clobber  : remove the entire build folder"
            echo "   examples : run the exampes"
            echo "   help     : show this help"
            
            exit 0
        ;;

        build)
            cd src && pwd && make && make unit

            shift
        ;;
        *)
            ./mk help
            exit 0
        ;;

    esac
done


