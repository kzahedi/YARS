#!/usr/bin/env python

import argparse, os, shutil

p = argparse.ArgumentParser(description="Arguments:")
p.add_argument("-f", type=str, help="Blender file.")
p.add_argument("-C", type=int, help="Number of CPUs.",                         default=2)
p.add_argument("-e", type=int, help="End frame.")
p.add_argument("-n", type=int, help="Step width.",                             default=4)
p.add_argument("-N", type=int, help="How many per file.",                      default=5)
p.add_argument("-c", type=str, help="Cluster [rzg, hydra] (default = hydra).", default="hydra")
args = p.parse_args()

filename = os.path.basename(args.f)
project_name = filename.split(".")[0]

rzg_blender_line ='''
blender -b #FILE# -a -s #START# -e #END# --render-output /ptmp/allg/zahedi/#OUTPUT#
'''
rzg_script =\
'''
#!/bin/sh

#$ -N #JOB NAME#
#$ -cwd
#$ -j y
#$ -o /afs/ipp/home/z/zahedi/#PRJ#/#JOB NAME#.out
#$ -l h_rt=172800,h_vmem=4G

module load blender

#RENDER LINES#
'''

hydra_blender_line ='''
if [ ! -f /clusterhome/zahedi/#PRJ#/#FRAME_PREFIX#.png ]; then
  /usr/people/zahedi/blender-2.72/blender -t #CPUS# -b /usr/people/zahedi/#PRJ#/#FILE# -f #FRAME#
fi
'''
hydra_script =\
'''
#$ -N #JOB NAME#_#PRJ#
#$ -cwd
#$ -j y
#$ -o /clusterhome/zahedi/#PRJ#/#JOB NAME#.out
#$ -l short,dedicated=#CPUS#

#RENDER LINES#

rm /usr/people/zahedi/#PRJ#/#SCRIPT_NAME#
'''

script = ""
blender_line = ""
if args.c == "rzg":
  script = rzg_script
  blender_line = rzg_blender_line
else:
  script = hydra_script
  blender_line = hydra_blender_line

def prefix(n):
  s = str(n)
  while len(s) < 4:
    s = "0" + s
  return s

print "Creating directory " + project_name
try:
  os.mkdir(project_name)
except Exception:
  print "Need to remove directory " + project_name + " first"
  shutil.rmtree(project_name)
  os.mkdir(project_name)

print "Copying " + args.f + " to " + project_name + "/" + filename
shutil.copyfile(args.f, project_name + "/" + filename)

index = 1
lines = []
current_line = ""
l = blender_line
l = l.replace("#FILE#", filename)
l = l.replace("#FRAME#", str(1))
l = l.replace("#CPUS#", str(args.C))
l = l.replace("#FRAME_PREFIX#", prefix(1))
l = l.replace("#PRJ#", project_name)
l = l.replace("#OUTPUT#", project_name)
current_line = current_line + "\n" + l
if index % args.N == 0:
  lines.append(current_line)
  current_line = ""
index = index + 1

for i in range(0, args.e + 1, args.n):
  l = blender_line
  l = l.replace("#FILE#", filename)
  l = l.replace("#FRAME#", str(i))
  l = l.replace("#FRAME_PREFIX#", prefix(i))
  l = l.replace("#PRJ#", project_name)
  l = l.replace("#OUTPUT#", project_name)
  l = l.replace("#CPUS#", str(args.C))
  current_line = current_line + "\n" + l
  if index % args.N == 0:
    lines.append(current_line)
    current_line = ""
  index = index + 1

if len(current_line) > 0:
  lines.append(current_line)

print "Number of lines: " + str(len(lines))

index = 1
for l in lines:
  s = script.replace("#JOB NAME#","b" + str(index))
  s = s.replace("#PRJ#", project_name)
  s = s.replace("#OUTPUT#", project_name)
  s = s.replace("#RENDER LINES#",l)
  s = s.replace("#CPUS#", str(args.C))
  s = s.replace("#SCRIPT_NAME#", "run_" + str(index) + ".sh")
  fd = open(project_name + "/run_" + str(index) + ".sh", "w")
  fd.write(s)
  fd.close()
  index = index + 1

os.system("tar -cjf " + project_name + ".tar.bz2 " + project_name)
os.system("rm -rf " + project_name)
