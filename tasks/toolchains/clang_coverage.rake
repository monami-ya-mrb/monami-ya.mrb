MRuby::Toolchain.new(:clang) do |conf|
  toolchain :gcc

  [conf.cc, conf.objc, conf.asm].each do |cc|
    cc.command = ENV['CC'] || 'clang'
    cc.flags << %w(-g -fprofile-arcs -ftest-coverage)
  end
  conf.cxx.command = ENV['CXX'] || 'clang++'
  conf.cxx.flags << %w(-g -fprofile-arcs -ftest-coverage)
  conf.linker.command = ENV['LD'] || 'clang'
  conf.linker.flags << %w(-g -fprofile-arcs -ftest-coverage --coverage)
end
