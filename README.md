Cinder OpenCV3 Sample with macOS
===

## Setup

```
# Download opencv 3.2.0 framework
curl -o libs/opencv2.framework.zip http://ibu.tokyo/downloads/opencv-3.2.0.framework.zip
unzip libs/opencv2.framework.zip -d libs


# Skip if you already have Cinder
# Download cinder
git submodule update --init --recursive

# Build cinder
./Cinder/proj/xcode/fullbuild.sh
```
