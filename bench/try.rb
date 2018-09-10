require_relative 'bench.rb'

puts(bench(100000) {
       begin
         raise 42
       rescue Exception => e
       end
     })
