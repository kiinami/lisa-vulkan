shaders = [
    'common.slang',
    'constants.slang',

    'depth.slang',
    'composition.deferred.slang',
    'geometry.deferred.slang',

    'forward.slang'
]
foreach s : shaders
    configure_file(input : s, output : s, copy : true)
endforeach
