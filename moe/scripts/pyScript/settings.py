import json

with open('scripts/configs/settings.json') as f:
    projSettings = json.load(f)


class TimeoutSettings:
    settings = projSettings['TimeoutSettings']
    default: float = settings['default']
    make: float = settings['make']
    cmake: float = settings['cmake']


nullDev = '/dev/null'
moecompilerPath = 'build/compiler'

if __name__ == '__main__':
    print(projSettings)
