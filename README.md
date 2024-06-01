# Source Code - libpirates.dll
This repository contains ready-to-use water code reverse engineered from libpirates.dll.

![](https://static.tlopo.com/static/img/posts/2a390be4c2584139b915c1e77319febe.jpg)

![](https://static.tlopo.com/static/img/posts/c7985a175eca4705962b1704b9b675cf.png)

## Background
This source was originally based on the water in a Pirates of the Caribbean cannon ride experience at a theme park. It later was implemented into Pirates of the Caribbean Online as the game's ocean wave calculation system.

## Requirements
This reverse engineering of the code is intended to be used alongside the latest version of the Panda3D Game Engine, which can be obtained here: https://github.com/Panda3D/Panda3D/

## Building
This subheading describes how to build libpirates alongside Panda3D.

### Update makepanda/makepanda.py

The following code must be added to `makepanda/makepanda.py`.

The goal is to build `libpirates` into the `panda3d.core` package, thereby allowing this code to be interrogated by Panda and referenced in Python code.

The code below is provided in the order that it will appear in `makepanda.py`. Search `makepanda.py` for similar nearby code, then add the code below to the relevant areas.

Upon completion, run `makepanda.py` to build Panda3d. The built image will have `libpirates` support. From there, you may simply import `SeaPatchNode` and `SeaPatchRoot` from `panda3d.core`.

```python
#
# DIRECTORY: panda/src/pirates/
#

OPTS=['DIR:panda/src/pirates', 'BUILDING:PANDA']
TargetAdd('p3pirates_composite1.obj', opts=OPTS, input='libpirates_composite.cxx')

OPTS=['DIR:panda/src/pirates', 'PYTHON']
IGATEFILES=GetDirectoryContents('panda/src/pirates', ["*.h", "*_composite.cxx"])
TargetAdd('libp3pirates.in', opts=OPTS, input=IGATEFILES)
TargetAdd('libp3pirates.in', opts=['IMOD:panda3d.core', 'ILIB:libp3pirates', 'SRCDIR:panda/src/pirates'])
PyTargetAdd('libp3pirates_igate.obj', opts=OPTS, input='libp3pirates.in')
```

**Note:** The `libpirates` directory included in this source repository should be copied into `panda/src/pirates`. This will allow makepanda to discover the files and compile them into `core`, which can later be used from within your Python panda application.

```python
TargetAdd('libpanda.dll', input='p3pirates_composite1.obj')
```

```python
PyTargetAdd('core_module.obj', input='libp3pirates.in')
```

```python
PyTargetAdd('core.pyd', input='libp3pirates_igate.obj')
```


## Example Usage


```python
# Must have built libpirates with Panda3D. Refer to the "Building"
# heading for information on how to do this in the README.
from panda3d.core import (
    NodePath,
    SeaPatchNode,
    SeaPatchRoot,
)

# Sample SeaPatch class loading "SeaPatchRoot" and "SeaPatchNode" and using their calculations.
class SeaPatch:
    def __init__(self):
        self.patch = SeaPatchRoot()
        self.patch.setSeaLevel(0)

        patchNode = SeaPatchNode(
            'seapatch',
            self.patch
        )
        patchNode.setWantReflect(0)
        self.patchNP = NodePath(patchNode)
        self.patchNP.setColorScale(0.15, 0.4, 0.5, 1.0)
        self.patchNP.setTwoSided(True)
        self.patchNP.hide()
        self.runDefaultConfig()

    def runDefaultConfig(self):
        self.patch.setOverallSpeed(0.6000)
        self.patch.setUvSpeed(Vec2(0.00270, 0.00127))
        self.patch.setThreshold(999.00)
        self.patch.setRadius(1000.00)
        self.patch.setUvScale(VBase2(36.0000, 36.0000))
        self.patch.setHighColor(Vec4(1.0, 1.0, 1.0, 1.0))
        self.patch.setLowColor(Vec4(0.1833, 0.1833, 0.1833, 0.1833))
        self.patch.enableWave(0)
        self.patch.setWaveTarget(0, SeaPatchRoot.WTZ)
        self.patch.setWaveFunc(0, SeaPatchRoot.WFSin)
        self.patch.setWaveDirection(0, Vec2(0.8288, 0.5595))
        self.patch.setChoppyK(0, 1)
        self.patch.setWaveAmplitude(0, 5.0800)
        self.patch.setWaveLength(0, 324.7441)
        self.patch.setWaveSpeed(0, 1.0000)
        self.patch.enableWave(1)
        self.patch.setWaveTarget(1, SeaPatchRoot.WTZ)
        self.patch.setWaveFunc(1, SeaPatchRoot.WFSin)
        self.patch.setWaveDirection(1, Vec2(0.9949, 0.1005))
        self.patch.setChoppyK(1, 4)
        self.patch.setWaveAmplitude(1, 1.2000)
        self.patch.setWaveLength(1, 96.5732)
        self.patch.setWaveSpeed(1, 4.0000)
        self.patch.enableWave(2)
        self.patch.setWaveTarget(2, SeaPatchRoot.WTZ)
        self.patch.setWaveFunc(2, SeaPatchRoot.WFSin)
        self.patch.setWaveDirection(2, Vec2(-0.5699, -0.8217))
        self.patch.setChoppyK(2, 1)
        self.patch.setWaveAmplitude(2, 7.5800)
        self.patch.setWaveLength(2, 208.0925)
        self.patch.setWaveSpeed(2, 1.2400)
        self.patch.enableWave(3)
        self.patch.setWaveTarget(3, SeaPatchRoot.WTZ)
        self.patch.setWaveFunc(3, SeaPatchRoot.WFSin)
        self.patch.setWaveDirection(3, Vec2(0.5331, -0.8460))
        self.patch.setChoppyK(3, 1)
        self.patch.setWaveAmplitude(3, 0.8800)
        self.patch.setWaveLength(3, 74.3260)
        self.patch.setWaveSpeed(3, 4.0000)
        self.patch.enableWave(4)
        self.patch.setWaveTarget(4, SeaPatchRoot.WTU)
        self.patch.setWaveFunc(4, SeaPatchRoot.WFNoise)
        self.patch.setChoppyK(4, 0)
        self.patch.setWaveAmplitude(4, 0.1000)
        self.patch.setWaveLength(4, 60.0000)
        self.patch.setWaveSpeed(4, 1.0000)
        self.patch.enableWave(5)
        self.patch.setWaveTarget(5, SeaPatchRoot.WTV)
        self.patch.setWaveFunc(5, SeaPatchRoot.WFNoise)
        self.patch.setChoppyK(5, 0)
        self.patch.setWaveAmplitude(5, 0.1000)
        self.patch.setWaveLength(5, 60.0000)
        self.patch.setWaveSpeed(5, 1.0000)
```
