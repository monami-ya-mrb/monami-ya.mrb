MRuby.each_target do
  if thread_binds.length != 0
    thread_build_dir = "#{build_dir}/threads"

    file "#{thread_build_dir}/mrb_thread_bind.cfg" => [MRUBY_CONFIG, __FILE__] do |t|
      FileUtils.mkdir_p thread_build_dir
      open(t.name, 'w') do |f|
        f.puts %q[#include "mrb_thread_bind.h"]
        f.puts %q[INCLUDE("\"mrb_thread_bind.h\"");]
        f.puts %q[ATT_INI({TA_HLNG, 0, mrb_thread_bind_initialize});]
        thread_binds.each_with_index do |t, i|
          if t.type == :task
            stack_size = t.stack_size
            stack_size = 16384 if stack_size < 16384
            priority = (t.priority <= 0) ? 1 : t.priority
            f.puts %Q[CRE_TSK(MRB_TASK#{i}, { TA_HLNG #{ "|TA_ACT" if t.activate }, #{i}, mrb_thread_bind_entry, #{priority}, #{stack_size}, NULL});]
          end
        end
      end
    end

    file "#{thread_build_dir}/mrb_thread_bind.h" => [MRUBY_CONFIG, __FILE__] do |t|
      FileUtils.mkdir_p thread_build_dir
      open(t.name, 'w') do |f|
        f.puts %Q[#include "itron.h"]
        f.puts %Q[#ifndef _MACRO_ONLY]
        f.puts %Q[extern void mrb_thread_bind_initialize(VP_INT exinf);]
        f.puts %Q[extern void mrb_thread_bind_entry(VP_INT exinf);]
        f.puts %Q[#endif]
      end
    end

    thread_binds.each_with_index do |thread, i|
      file "#{thread_build_dir}/thread_script_#{i}.cinc" =>
        ["#{root}/#{thread.script_path}"] do |t|
        FileUtils.mkdir_p thread_build_dir
        open(t.name, 'w') do |f|
          mrbc.run f, t.prerequisites, "mrb_thread_script#{i}"
        end
      end
    end

    file "#{thread_build_dir}/mrb_thread_bind.c" =>
      (0..(thread_binds.length - 1)).map { |n| "#{thread_build_dir}/thread_script_#{n}.cinc" } + [ __FILE__ ] do |t|
      FileUtils.mkdir_p thread_build_dir
      open(t.name, 'w') do |f|
        f.puts %Q[#include "itron.h"]
        f.puts %Q[#include "mruby.h"]
        f.puts %Q[#include "mruby/irep.h"]
        (0..(thread_binds.length - 1)).each do |i|
          f.puts %Q[#include "#{thread_build_dir}/thread_script_#{i}.cinc"]
        end
        f.puts
        f.puts %Q[static const uint8_t *thread_scripts[] = {]
        thread_binds.each_with_index do |t, i|
          f.puts %Q[  mrb_thread_script#{i},]
        end
        f.puts %Q[  NULL]
        f.puts %Q[};]
        f.puts
        f.puts %Q[static mrb_state *mrb[#{thread_binds.length}];]
        f.puts
        f.puts %Q[void]
        f.puts %Q[mrb_thread_bind_initialize()]
        f.puts %Q[{]
        thread_binds.each_with_index do |t, i|
          if t.sandbox
            idx = sandboxes.names.index(t.sandbox.name)
            unless i
              fail %Q[Specify unavailable sandbox "#{sandbox.name} to a thread.]
            end
            sandbox_id = idx + 1
          else
            sandbox_id = 0
          end
          f.puts %Q[  mrb[#{i}] = mrb_open_sandbox(#{sandbox_id});]
        end
        f.puts %Q[}]
        f.puts
        f.puts %Q[void mrb_thread_bind_entry(VP_INT exinf)]
        f.puts %Q[{]
        f.puts %Q[  mrb_load_irep(mrb[(size_t)exinf], thread_scripts[(size_t)exinf]);]
        f.puts %Q[}]
      end
    end

    file objfile("#{thread_build_dir}/mrb_thread_bind") =>
      ["#{thread_build_dir}/mrb_thread_bind.c",
       "#{thread_build_dir}/mrb_thread_bind.h",
       "#{thread_build_dir}/mrb_thread_bind.cfg"]

    self.libmruby << objfile("#{thread_build_dir}/mrb_thread_bind")
  end
end
