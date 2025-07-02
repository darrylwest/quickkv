# What to do with Artifacts

## Define destination directory

INSTALL_DIR="/opt/my-app"

## Unpack the artifact into a temporary location

The -C flag tells tar to change to that directory before extracting
sudo mkdir -p $INSTALL_DIR
sudo tar xzvf /tmp/my-app-v1.0.tar.gz -C $INSTALL_DIR --strip-components=1

## Optional: Set correct permissions
sudo chown -R root:root $INSTALL_DIR

## Optional: If you need to restart a service that uses this library

sudo systemctl restart my-dependent-service

## Clean up the temporary tar file
rm /tmp/my-app-v1.0.tar.gz

