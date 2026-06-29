project('image-preprocess', 'cpp', version: '0.1.0',
  default_options: ['cpp_std=c++17', 'warning_level=2'])

sdk_sp = subproject('satellite-plugin-sdk')
satellite_common_dep = sdk_sp.get_variable('satellite_common_dep')

executable('image-preprocess', 'src/image-preprocess/main.cpp',
  dependencies: [satellite_common_dep], install: true)

install_data('configs/plugins/image-preprocess.json',
  install_dir: get_option('datadir') / 'satellite/plugins')
