#! /bin/bash
BUILD_DIR=$HOME/liga-release

#BUILD_DIR=/home/user/projects/control_app_liga/pg-liga/build-liga-Desktop-Release
echo "BUILD_DIR=$BUILD_DIR"
QT_DEPLOY=$HOME/bin/linuxdeployqt

BRANCH=$(git symbolic-ref --short -q HEAD)
if [ "$BRANCH" == "stable" ]
    REPOS_DIR=$HOME/bin/liga/deploy/release
then                  
    REPOS_DIR=$HOME/bin/liga/deploy/testing
fi

#current_date=$(date +%Y.%m.%d)
#echo $current_date
rev_list=$(git --git-dir="$PWD/.git" rev-list HEAD --count)

DIRNAME="liga-msa-1.1.$rev_list"
TARGET_DIR="$REPOS_DIR/$DIRNAME"


if [ -d $TARGET_DIR ]; then
    echo "Path exist, success"
else 
    echo "Not exist"
    mkdir -p $TARGET_DIR
fi

echo $REPOS_DIR
echo $DIRNAME
echo $TARGET_DIR


echo 'Writing changelog...'
changelog_file="changelog.txt"
if [ ! -f $TARGET_DIR/$changelog ]; then
    touch $TARGET_DIR/$changelog
fi
echo "$DIRNAME последние изменения (ревизия $rev_list)" > "$TARGET_DIR/$changelog_file"
echo -e "\n" >> "$TARGET_DIR/$changelog_file"
echo -e "$(git --git-dir="$PWD/.git" log | grep Author -v)" >> "$TARGET_DIR/$changelog_file"

echo $PWD
echo 'Copying experiment app...'
cp -p "$BUILD_DIR/experiment" "$TARGET_DIR/."
echo 'Copying modbus app...'
cp -p "$BUILD_DIR/modbus-server" "$TARGET_DIR/."
echo 'Copying experiment-manager app...'
cp -p "$BUILD_DIR/experiment-manager" "$TARGET_DIR/."
echo 'Copying default config.json ...'
cp -p "$PWD/config.json" "$TARGET_DIR/."

cd $TARGET_DIR

$QT_DEPLOY experiment
$QT_DEPLOY modbus-server
$QT_DEPLOY experiment-manager

mkdir experiments
mkdir protocols
mkdir upgrades

