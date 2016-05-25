
exit unless ARGV.first

file = File.read(ARGV.first).split("\n\n")
data = file.map { |elem| elem.split("\n").map { |e| e.split(",").last } }
puts data.transpose.map { |d| d.join(",") }

