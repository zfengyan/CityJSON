# First

First input:
myfile.city.json
myfile.triangulated.city.json

First output:
myfile.output.first.city.json

# Second

Second input:
myfile.input.second.city.json -- the same file as First output
myfile.input.second.triangulated.city.json

Second output:
myfile.output.second.city.json

# error process

Error process input:
myfile.error.process.input.json -- the same file as Second output

Error process output:
myfile.result.city.json -- result file

# Attention

in the [main.cpp -> line 1070](https://github.com/SEUZFY/CityJSON/blob/master/src/main.cpp), the second input requires the 

generated file `myfile.input.second.city.json` of the first output and its corresponding triangulated file `myfile.input.second.triangulated.city.json`

as second input files, thus for the first run (to obtain the first output files), you may be willing to comment the code below the `//Second input`, and

after first output files are obtained, you could uncomment those to let the program finish the second output.

As for the error process code starting from [main.cpp -> line 1136](https://github.com/SEUZFY/CityJSON/blob/master/src/main.cpp) the related files of error 

report are also required. 
