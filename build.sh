#!/bin/bash

export ARCH=arm64
export CROSS_COMPILE=aarch64-none-linux-gnu-
export KERNEL_DIR=/home/jiamuyu-server/workspace/Project/x5-rdk-gen/source/kernel

cmd=$1
target=$2
module=$3

current_dir=$(pwd)
install_dir=/lib/modules/6.1.83/user
remote_server=10.10.91.129

function verify_target()
{
    if [ ! -d "$target" ]; then
        echo "错误：目录 '$target' 不存在！" >&2
        exit 1
    fi
}

function show_help()
{
    echo "Usage: ./build.sh [build/clean/install] target [user/kernel/all]"
}

function build_target_kernel()
{
    echo "build linux module: '$target'"
    cd ${current_dir}/${target}/kernel
    make
    mkdir -p ${current_dir}/out
    mv ${current_dir}/${target}/kernel/*.ko ${current_dir}/out
}

function build_target_user()
{
    echo "build user module: '$target'"
    cd ${current_dir}/${target}/user
    make
    mkdir -p ${current_dir}/out
    mv ${current_dir}/${target}/user/app ${current_dir}/out
}

function install_product()
{
    echo "install build product to remote server"
    scp ${current_dir}/out/* root@${remote_server}:${install_dir}
}

function clean_kernel()
{
    echo "clean kernel space"
    cd ${current_dir}/${target}/kernel
    make clean
    rm -f ${current_dir}/out/*.ko
}

function clean_user()
{
    echo "clean user space"
    cd ${current_dir}/${target}/user
    make clean
    rm -f ${current_dir}/out/app
}

if [[ "${cmd}" == "build" ]]; then
    verify_target
    if [[ "${module}" == "kernel" ]]; then
        build_target_kernel
    elif [[ "${module}" == "user" ]]; then
        build_target_user
    elif [[ "${module}" == "all" ]]; then
        build_target_kernel
        build_target_user
    else
        echo "Unsupported module!"
        show_help
    fi
elif [[ "${cmd}" == "clean" ]]; then
    verify_target
    if [[ "${module}" == "kernel" ]]; then
        clean_kernel
    elif [[ "${module}" == "user" ]]; then
        clean_user
    elif [[ "${module}" == "all" ]]; then
        clean_kernel
        clean_user
    else
        echo "Unsupported module!"
        show_help
    fi
elif [[ "${cmd}" == "install" ]]; then
    install_product
else
    echo "Unsupported cmd!"
    show_help
fi
