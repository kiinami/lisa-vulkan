shader_out = meson.current_build_dir()

shader_dirs = {
    'lib' : ['common.slang', 'constants.slang'],
    'preprocess' : ['depth.slang', 'ssao.slang', 'shadow.slang'],
    'deferred' : ['composition.slang', 'geometry.slang'],
    'forward' : ['forward.slang'],
    'postprocess' : ['convolutional.slang'],
}

shader_targets = []

fs = import('fs')

foreach dir, names : shader_dirs
    foreach name : names
        shader_targets += custom_target(
            dir + '_' + name,
            input : dir / name,
            output : dir + '_' + name + '.slang',
            command : [
                find_program('python3', 'python'),
                '-c',
                'import sys, shutil, os; os.makedirs(sys.argv[1], exist_ok=True); shutil.copy(sys.argv[2], sys.argv[3])',
                shader_out / dir,
                '@INPUT@',
                shader_out / dir / name,
            ],
            build_by_default : true
        )
    endforeach
endforeach

shader_dep = declare_dependency(sources : shader_targets)