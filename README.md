# KinectCombineDepth

openframerorks

### setting

#### addonフォルダに追加

- [ofxKinectV2](https://github.com/ofTheo/ofxKinectV2)
- [ofxTurboJpeg](https://github.com/armadillu/ofxTurboJpeg)
- [ofxDatGui](https://github.com/braitsch/ofxDatGui)

#### Build Phases > Linked Frameworks and Librariesに追加

- OpenCL.framework
- libturbojpeg.dylib from ofxTurboJpeg/libs/turbo-jpeg/lib/osx


#### Build Phases > Copy Filesにlibturbojpeg.dylibを追加、ターゲットは"Framework"

#### Build Settings > Search Paths > Library Search PathsにPathを追加

	`../../../addons/ofxTurboJpeg/libs/turbo-jpeg/lib/osx`

