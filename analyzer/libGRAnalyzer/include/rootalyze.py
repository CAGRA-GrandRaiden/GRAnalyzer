import cog
import hashlib
import os,tempfile

def readcmd(cmd):
    ftmp = tempfile.NamedTemporaryFile(suffix='.out', prefix='tmp', delete=False)
    fpath = ftmp.name
    if os.name=="nt":
        fpath = fpath.replace("/","\\") # forwin
    ftmp.close()
    os.system(cmd + " > " + fpath)
    data = ""
    with open(fpath, 'r') as file:
        data = file.read()
        file.close()
    os.remove(fpath)
    return data

def get_vars(filename):
    members = []
    for line in open(filename):
        if 'DST_VAR' in line:
            line = line.split()
            members = line[4].split(':')
            break
    members[0] = members[0][1:]
    members[-1] = members[-1][:-1]
    return members

def md5(string):
    m = hashlib.md5()
    m.update(string)
    return m.hexdigest()

def check_dst_vars(filename):
    for line in open(filename):
        line = line.strip()
        if "DST_VAR" in line:
            #checksum = md5(line)
            checksum = line
            break
    return checksum

def definitions():
    members = get_vars('../hist.def')
    for i,fn in enumerate(members):
        #cog.outl("int i{}; //! ignored by root".format(fn))
        cog.outl("double {0}(const int& i) {{ return (data.count({1}) > 0) ? data[{1}][i] : BAD_NUM; }}".format(fn,i))
        #cog.outl("double {0}() {{ return (data.count({1}) > 0) ? data[{1}][0] : BAD_NUM; }}".format(fn,i))
        cog.outl("vector<double>* {0}() {{ return (data.count({1}) > 0) ? &data[{1}] : nullptr; }}".format(fn,i))
    # assume that Make is run from the analyzer directory and hist.def is one level up
    fullpath = readcmd('readlink -f ../hist.def')[:-1]
    astr = """
static void HistDefCheckSum(int pid=0) {{\n\n  string record = "{0}";""".format(check_dst_vars(fullpath).replace("\"","\\\"")) + """
  string line, dstvars = "DST_VAR";\n"""+"""
  ifstream histdef ("{0}");""".format(fullpath) + """
  if (histdef.is_open())
  {
    while ( getline (histdef,line) )
    {
      if (line.find(dstvars) != string::npos) {
        break;
      }
    }
    histdef.close();
  }
  line.erase(line.begin(), std::find_if(line.begin(), line.end(), std::not1(std::ptr_fun<int, int>(std::isspace))));
  line.erase(std::find_if(line.rbegin(), line.rend(), std::not1(std::ptr_fun<int, int>(std::isspace))).base(), line.end());
  fprintf(stderr,"%s\\n",line.c_str());
  if (line == record) {
    return;
  } else {
    fprintf(stderr,"%s vs %s\\n",record.c_str(),line.c_str());
    const char* msg =
      "\\n###################### RUNTIME ERROR ######################\\n"
      "DST_VAR line in hist.def has changed since compilation.\\n"
      "Please recompile the analyzer before attempting to run.\\n"
      "###########################################################\\n";
    fprintf(stderr,"%s",msg);
    kill(pid,SIGINT);
    throw runtime_error(msg);
  }
}"""
    cog.outl(astr)
