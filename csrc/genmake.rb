#!/usr/local/bin/ruby

cpp_files = Dir['*.cpp']
obj_files = cpp_files.collect{|x| x.sub(/\.cpp$/, '.o')}

File.open('Makefile', 'w+') do |f|
    f.puts <<ALL

all: v

v: #{obj_files.join(' ')}
\tg++ -o v #{obj_files.join(' ')}

ALL
    cpp_files.each do |cpp|
        depends = `g++ -M #{cpp}`
        cdep = depends.gsub(/\\*\n/, ' ').split(/ +/).delete_if {|x| x =~ /\//}
        f.puts cdep.join(' ')
        f.puts "\tg++ -c -g #{cpp}"
        f.puts "\n"
    end
    
    f.puts <<TEST

test:
\t./v ../scripts/test.v

TEST
    f.puts <<CLEAN
clean:
\trm *.o

CLEAN

end
