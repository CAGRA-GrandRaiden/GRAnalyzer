import cog

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


# def initialization():
#     members = get_vars('../hist.def')
#     for i,var in enumerate(members):
#         if i<len(members)-1:
#             cog.outl("i{}(-1),".format(var))
#         else:
#             cog.outl("i{}(-1)".format(var))

def definitions():
    members = get_vars('../hist.def')
    for i,fn in enumerate(members):
        #cog.outl("int i{}; //! ignored by root".format(fn))
        cog.outl("double {0}(const int& i) {{ return (data.count({1}) > 0) ? data[{1}][i] : BAD_NUM; }}".format(fn,i))
        cog.outl("double {0}() {{ return (data.count({1}) > 0) ? data[{1}][0] : BAD_NUM; }}".format(fn,i))

