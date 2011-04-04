############################################################
#
# do_trend_html.py
#
############################################################
#
# Author: Seb Viret <viret@in2p3.fr>
#
# November 23, 2010
#
# Goal: Creates an html page to display the results 
#       for trend plots
#
# Name of the page: MIB_monitor.html
#
# For more info, have a look at the CMS MIB webpage:
# 
# http://sviret.web.cern.ch/sviret/Welcome.php?n=CMS.MIB
#
#############################################################

from src.GenericWorker import *
import time
import math

class do_trend_html(GenericWorker):
    "Compute history plot"

    def __init__(self,typecut=""):

        # We start to write the html code

        self.typecut  = typecut
        self.filename = "MIB_monitor.html"
        self.run_list = []
        
        self.htm = open(self.filename,"w")

        self.htm.write("<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.0 Transitional//EN\" \"http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd\">\n")
        self.htm.write("<html xmlns=\"http://www.w3.org/1999/xhtml\" xml:lang=\"en\" lang=\"en\">\n")
        self.htm.write("<head>\n")
        self.htm.write(" <title>Beam background monitoring</title>\n")
        self.htm.write(" <meta http-equiv='Content-Style-Type' content='text/css' />\n")
        self.htm.write(" <style type='text/css'></style>\n")  
        self.htm.write(" <meta name='robots' content='index,follow' />\n")
        self.htm.write(" <link rel='stylesheet' type='text/css' href='http://sviret.web.cern.ch/sviret/pub/skins/monobook/monobook.css' />\n")
        self.htm.write(" <link rel='stylesheet' type='text/css' href='http://sviret.web.cern.ch/sviret/pub/css/local.css' />\n")
        self.htm.write(" <style type='text/css'>#header { border-bottom: none; }</style>\n")
        self.htm.write("</head>\n")
        self.htm.write("\n")
        self.htm.write("<body><a name='monobook_topofpage'></a>\n")
        self.htm.write("<div id='globalwrapper'>\n")
        self.htm.write("<div id='pageleft'>\n")
        self.htm.write("\n")
        self.htm.write("<div id='pageleftcontent'>\n")
        self.htm.write("<div class='pageleftbody' id='sidebar'>\n")
        self.htm.write("<p class='sidehead'>\n")
        self.htm.write("  <a class='wikilink' href='http://sviret.web.cern.ch/sviret/Welcome.php?n=Main.HomePage'>Seb's Page</a>\n")
        self.htm.write("</p>\n")
        self.htm.write("<ul>\n")
        self.htm.write("<li><a class='wikilink' href='http://sviret.web.cern.ch/sviret/Welcome.php?n=CMS.MIB'><strong>Back to LHC background</strong></a></li>\n")
        self.htm.write("</ul>\n")
        self.htm.write("</div>\n")        
        self.htm.write("</div><!-- id='pageleft_content' -->\n")
        self.htm.write("\n")
        self.htm.write("<div id='pagelogo'>\n")
        self.htm.write("  <a style='background-image: url(http://sviret.web.cern.ch/sviret/pub/skins/pmwiki/pmwiki-32.gif);'\n")
        self.htm.write("     href='http://sviret.web.cern.ch/sviret/Welcome.php' title=\"Seb Viret personal page - BBM 138750\"></a>\n")
        self.htm.write("</div><!-- id='pagelogo' -->\n")
        self.htm.write("</div><!--/PageLeftFmt-->\n")
        self.htm.write("\n")
        self.htm.write("<a name='topcontent'></a>\n")
        self.htm.write("<div id='content'>\n")
        self.htm.write("  <div id='header'></div>\n") 
        self.htm.write("  <div id='tabpage'>\n")
        self.htm.write("    <div id='contentbody'>\n")
        self.htm.write("\n")         
        self.htm.write("      <div id='wikitext'>\n")
        self.htm.write("	<h1>Standard trend plots </h1>\n")

        self.htm.write("	<p>This page presents the MIB monitoring results over time</p>\n")
        self.htm.write("	<h2><a target='_blank'  class='wikilink' href='http://sviret.web.cern.ch/sviret/Welcome.php?n=CMS.MIBMonitorHowTo'>How to read this page ?</a></h2>\n")

        
        self.htm.write("	<h2>Used runs list</h2>\n")

        self.htm.write("<p class='vspace'>\n")
        self.htm.write("<span id='run_part-tog' class='toggle'><input type='button' class='inputbutton togglebutton' value='usedRunList' onclick=\"javascript:toggleObj('run_part','show','usedRunList','Hide','','','','1')\" /><style type='text/css'> #run_part { display:none; }  @media print { #run_part {display:block} } </style></span>\n")
        self.htm.write("</p>\n")
        self.htm.write("<div  id='run_part' > \n")
        self.htm.write("<p>For each run you got two links. If you click on the run number you have access to the CMS official info concerning this run (WBM). If you click on background info, you will be directed towards a page provided a more detailed analysis of the run MIB rates.</p>\n")
        self.htm.write("<ul>\n")

        
    def ProcessRegion(self, region):
                          
        # First retrieve all the relevant partition infos

        for event in region.GetEvents():


            if not event.data.has_key('is_OK') or event.data['t_start']==0:
                continue
    
            if event.runNumber not in self.run_list:
                self.run_list.append(event.runNumber)


    def ProcessStop(self):

        self.run_list.sort()

        for run in self.run_list:

            text="<li>Run <a target='_blank'  class='urllink' href='https://cmswbm.web.cern.ch/cmswbm/cmsdb/servlet/RunSummary?RUN=%d' title='' rel='nofollow'><strong>%d</strong></a>,  <a target='_blank'  class='urllink' href='http://sviret.web.cern.ch/sviret/Images/CMS/MIB/Monitor/2011/R%s%d/Run_%d_background_DQM.html' title='' rel='nofollow'>background info</a></li>\n"%(run,run,self.typecut,run,run)
            self.htm.write(text)

        self.htm.write("</ul>\n")
        self.htm.write("</div>\n")

        self.htm.write("<br><div class='vspace'>\n")
        self.htm.write("</div><h2>Evolution of the prescale factor for algo bit 4 (BeamGas_BSC )and bunch number</h2>\n")
        self.htm.write("<p>The precise evaluation of the rates is strongly depending on the value of the prescale factor applied. A large prescale will induce less precision in the final result. </p>\n")

        self.htm.write("<p>We also present here a figure showing the evolution of the number of (non)-colliding bunches along time. Rates should not depend on that, but they can be contaminated by effects coming from this increase.</p><br>\n")

        self.htm.write("<div id='equation'>\n")
        self.htm.write("<table class='center'>\n")
        self.htm.write("<tbody>\n")
        self.htm.write("   <tr>\n")
        self.htm.write("<td>\n")
        self.htm.write("<a target='_blank' class='urllink' href='prescale_4_history.png'>\n")
        self.htm.write("<img id='fig2' src='prescale_4_history.png' alt=''></a>\n")
        self.htm.write("<div id='FigTitle'>1. Evolution of the prescale factor applied to bit 4 along time.</div>\n")
        self.htm.write("</td>\n")
        self.htm.write("<td>\n")
        self.htm.write("<a target='_blank' class='urllink' href='bunch_evolution.png'>\n")
        self.htm.write("<img id='fig2' src='bunch_evolution.png' alt=''></a>\n")
        self.htm.write("<div id='FigTitle'>2. Evolution of the number of colliding (black) and non-colliding (red) bunches along time.</div>\n")
        self.htm.write("</td>\n")
        self.htm.write("</tr>\n")
        self.htm.write("</tbody>\n")
        self.htm.write("</table>\n")
        self.htm.write("</div>\n")
        self.htm.write("<div class='vspace'></div>\n")

                    
        self.htm.write("<h2>Rate evolution along time</h2>\n")
        self.htm.write("<p>Here we provide, for all the algo bits involved in the MIB rate estimation (3/4/5/8), the normalized rate evolution obtained using the runs mentionned before. </p>\n")

        for i in range(6):

            if i==3 or i==4:
                continue
            
            bitname = "%d"%(3+i)
            self.WriteInfo(bitname)

        self.htm.write("  </div>\n")

        self.htm.write("<p class='vspace'>...This page was generated automatically on <strong>%s</strong>...</p>\n"%ROOT.TDatime().AsString())

        
        self.htm.write("      <span style='clear:both;'></span>\n")
        self.htm.write("    </div><!-- id='contentbody' -->\n")
        self.htm.write("  </div><!-- id='tabpage' -->\n")
        
 
        self.htm.write("                  </div>\n")      
        self.htm.write("      <span style='clear:both;'></span>\n")
        self.htm.write("    </div><!-- id='contentbody' -->\n")
        self.htm.write("  </div><!-- id='tabpage' -->\n")
        self.htm.write("</div><!-- id='content' -->\n")
        self.htm.write("</div><!-- id='globalwrapper' -->\n")
        self.htm.write("<!--HTMLFooter-->\n")

        self.htm.write("	<script type=\"text/javascript\"><!--\n")
        self.htm.write("		function toggleObj(obj, tog, show, hide, swap, set, cname, button) {\n")
        self.htm.write("			var e = document.getElementById(obj);\n")
        self.htm.write("			if (hide && swap!='') var e2 = document.getElementById(swap);\n")
        self.htm.write("			var text    = document.getElementById(obj + \"-tog\");\n")
        self.htm.write("			if (set=='1') document.cookie=cname+'='+tog+'; path=/';\n")
        self.htm.write("			if (tog=='show') {\n")
        self.htm.write("				e.style.display = 'block';\n")
        self.htm.write("				if(swap!='') e2.style.display = 'none';\n")
        self.htm.write("				var label = hide;\n")
        self.htm.write("				tog='hide';\n")				
        self.htm.write("			}\n")
        self.htm.write("			else {\n")
        self.htm.write("				e.style.display = 'none';\n")
        self.htm.write("				if(swap!='') e2.style.display = 'block';\n")
        self.htm.write("				var label = show;\n")
        self.htm.write("				tog='show';\n")
        self.htm.write("			}\n")
        self.htm.write("         var act = '\"javascript:toggleObj(\\''+obj+'\\',\\''+tog+'\\',\\''+show+'\\',\\''+hide+'\\',\\''+swap+'\\',\\''+set+'\\',\\''+cname+'\\',\\''+button+'\\');\"';\n")
        self.htm.write("         if (button==1)\n")
        self.htm.write("         	copy = '<input type=\"button\" class=\"inputbutton togglebutton\" value=\"'+label+'\" onclick='+act+' />';\n")
        self.htm.write("         else\n")
        self.htm.write("         	var copy = '<a class=\"togglelink\" href='+act+'>'+label+'</a>';\n") 
        self.htm.write("         text.innerHTML = copy; \n")  
        self.htm.write("      }\n")
        self.htm.write("   --></script>\n")



        
        self.htm.write("</body>\n")
        self.htm.write("</html>\n")
        self.htm.close()



    def WriteInfo(self,bitname):


        
        text="<ul><li>Algo bit %s rate<br /></li></ul>\n"%(bitname)
        
        self.htm.write(text)

        text="<span id='b%s-tog' class='toggle'><input type='button' class='inputbutton togglebutton' value='See' onclick=\"javascript:toggleObj('b%s','show','See','Hide','','','','1')\" /><style type='text/css'> #b%s { display:none; }  @media print { #b%s {display:block} } </style></span>\n"%(bitname,bitname,bitname,bitname)
        
        self.htm.write(text)

        text="<div  id='b%s' > \n"%(bitname)
        
        self.htm.write(text)

        self.pixcuts=[0,3,10,30,100]


        self.htm.write("<div id='equation'>\n")
        self.htm.write("<table class='center'>\n")
        self.htm.write("<tbody>\n")
	self.htm.write("   <tr>\n")

        for i in range(5):

            self.htm.write("<td>\n")

            text="<a target='_blank' class='urllink' href='bit_%s_history_%d_1.png'>\n"%(bitname,i)
            self.htm.write(text)
            
            text="<img id='figd' src='bit_%s_history_%d_1.png' alt=''></a>\n"%(bitname,i)
            self.htm.write(text)

            text="<div id='FigTitle'>%d. Rate evolution along time with at least %d pixel cluster(s).</div>\n"%(i,self.pixcuts[i])
            self.htm.write(text)
            
            self.htm.write("</td>\n")

        self.htm.write("	   </tr>\n")
        self.htm.write("   </tbody>\n")
        self.htm.write("   </table>\n")   
        self.htm.write("   </div>\n")
        self.htm.write("<br />\n")
        self.htm.write("</div>\n")
        self.htm.write("<div class='vspace'></div>\n")
