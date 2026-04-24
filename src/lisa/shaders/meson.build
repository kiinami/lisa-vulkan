shader_out = meson.current_build_dir()

shader_dirs = {
    'lib' : ['common.slang', 'constants.slang'],
    'preprocess' : ['depth.slang', 'ssao.slang'],
    'deferred' : ['composition.slang', 'geometry.slang'],
    'forward' : ['forward.slang'],
    'postprocess' : ['convolutional.slang'],
}

shader_targets = []

foreach dir, names : shader_dirs
    foreach name : names
        shader_targets += custom_target(
            dir + '_' + name,
            input : dir / name,
            output : dir + '_' + name,
            command : [
                'sh', '-c',
                'mkdir -p ' + shader_out / dir + ' && cp @INPUT@ ' + shader_out / dir / name
            ],
            build_by_default : true
        )
    endforeach
endforeach

shader_dep = declare_dependency(sources : shader_targets)