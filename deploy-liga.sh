#! /bin/bash
CURRENT_PATH=$(realpath $0)

TARGET=$(realpath $1)

echo "$CURRENT_PATH"

echo "Target=$TARGET"

current_date=$(date +%Y.%m.%d)
current_revision=$(git --git-dir="$src_dir/.git" rev-list HEAD --count)


if [ -d $TARGET ]; then
    echo "Path exist, success"
else 
    echo "Not exist"
    mkdir -p $TARGET
fi

echo $current_revision
