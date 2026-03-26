project('STC8H1K08 Samples',
        'c',
        version : '0.1',
        license : ['Apache-2.0']
)

# Use SDCC as compiler and linker
cc = find_program('sdcc', required : true)
stcgal = find_program('stcgal', required : true) # Install requirements.txt and run from venv

# Compile commands for sdcc
cc_args = ['-mmcs51',
           '--Werror',
           '--std-c2x',
           '--out-fmt-ihx',
           '--opt-code-size',
           '--fomit-frame-pointer']
# Link commands for sdcc
cc_incs = ['-I'+meson.current_source_dir() + '/inc']

# Flashing arguments for STCGAL
stcgal_args = ['-P', 'stc8g', '-p', '/dev/ttyUSB0']

# Since SDCC is not natively supported in meson, make it a generator...
compiler = generator(cc,
    output : '@BASENAME@.rel',
    arguments : cc_args + cc_incs + ['-c', '@INPUT@'] + ['-o', '@OUTPUT@'],
)

# Laundry list of example
progs = [
    ['hello', 'hello.hex', ['src/hello.c',
                            'src/hardware.c',
                            'src/usart.c',
                            'src/putchar.c',
                            'src/i2c.c'],
     'Hello World Example'],
]

# Build automation
foreach p : progs
    obj = compiler.process(p[2])
    exe = custom_target(p[1],
        input : obj,
        output : p[1],
        install : true,
        install_dir: 'firmware',
        command : [cc, cc_args, '-o', '@OUTPUT@', '@INPUT@'],
    )
    fls = run_target('flash_@0@'.format(p[0]),
        command : [stcgal] + stcgal_args + ['@0@'.format(exe.full_path())],
        depends : exe,
    )

endforeach
