# hist.awk 
# Histogram Definition Converstion Program
# Version 1.00  04-JUN-2000 by A.Tamii
# Usage: awk -f hist.awk input_file
BEGIN{
  i_index = 0;
  FUNCS = "dr_set:dr_get:dr_append:dr_max:dr_min:dr_nval:dr_get_vrec:dr_get_sval";
  split(FUNCS,funcs,":")
  line = 0;
}
{
    line++;
    str = $0;
    for(i in funcs){
	re = funcs[i] "\\("
	while(match(str,re)){
	    s1 = substr(str,1,RSTART-1)
	    s2 = substr(str,RSTART+RLENGTH)
	    if(match(s2,/^[^,)]*/)){
		var = substr(s2,RSTART,RLENGTH);
		s2 = substr(s2,RSTART+RLENGTH);
#		if(match(var,/\[[^[]*\]/)){
#		    ind = substr(var,RSTART+1,RLENGTH-2);
#		    var = substr(var,1,RSTART-1);
#		    sm = funcs[i]"_r(&dr_data,&dr_data_index[" i_index++ "],\"" \
#			var"\","ind
#		}else{
#		    sm = funcs[i]"_r(&dr_data,&dr_data_index[" i_index++ "],\"" \
#			var"\",0"
#		}
		level = 0;
		ind = 0;
		nam = var;
		n = length(nam);
		for(j=1;j<=n;j++){
		    c = substr(nam,j,1);
		    if(c=="["){
			if(level==0){
			    var = substr(nam,1,j-1);
			    ids = j;
			}
			level++;
		    }else if(c=="]"){
			level--;
			if(level==0){
			    ide = j;
			    ind = substr(nam,ids+1,ide-ids-1);
			    for(;j<=n;j++){
				c = substr(nam,j,1);
				if(c!=" " && c!="\t") break;
			    }
			}
		    }
		}
		if(level!=0 || j<n){
		    print "Error in line: " line > "/dev/stderr"
		    print $0 > "/dev/stderr"
		    exit(1);
		}
		sm = funcs[i]"_r(&dr_data,&dr_data_index[" i_index++ "],\"" \
		    var"\","ind
		str = s1 sm s2;
	    }else{
		print "Error in line: " line > "/dev/stderr"
		print $0 > "/dev/stderr"
		exit(1);
	    }
	}
    }
    print str;
}
END{
  print "static int dr_data_index["i_index"];"
}
