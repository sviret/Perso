############################################################
#
# do_single_run_html.py
#
############################################################
#
# Author: Seb Viret <viret@in2p3.fr>
#
# November 23, 2010
#
# Goal: Creates an html page to display the results 
#       for run ******
#
# Name of the page: Run_******_background_DQM.html
#
# Input parameters are:
#
# -> runnum: the CMS run number
#   DEFAULT is 143187
#
# For more info, have a look at the CMS MIB webpage:
# 
# http://sviret.web.cern.ch/sviret/Welcome.php?n=CMS.MIB
#
#############################################################

from src.GenericWorker import *
import time
import math

class do_single_run_html(GenericWorker):
    "Compute history plot"

    def __init__(self, runnum=143187):
        self.run      = runnum            
        self.BBTool   = MIBTools()
        self.doItonce = False

        self.nLS      = 0
        self.nBx1     = 0        
        self.nBx2     = 0
        self.nBxc     = 0        
        self.rDur     = 0        

        self.presc_t  = [0 for x in range(64)]
        self.presc_a  = [0 for x in range(128)]
        
        self.rates_B1 = [0 for x in range(6)]
        self.rates_B2 = [0 for x in range(6)]

        self.prescale = 666

        # We start to write the html code
        
        self.filename="Run_%d_background_DQM.html"%(self.run)

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
        
        text = "	<h1>Standard analysis for run <strong>%d</strong></h1>\n"%self.run
        self.htm.write(text)
        
        self.htm.write("	<h2>Basic information</h2>\n")
        text = "	<p>Information for this run is available on WBM by following <a target='_blank'  class='urllink' href='https://cmswbm.web.cern.ch/cmswbm/cmsdb/servlet/RunSummary?RUN="
        text_s = "%s%d' title='' rel='nofollow'>this link</a>.<br /><br />\n"%(text,self.run)            
        self.htm.write(text_s)

        
    def ProcessRegion(self, region):
                          
        # First retrieve all the relevant partition infos


        for event in region.GetEvents():

            if not event.data.has_key('is_OK'):
                continue
    
            if event.runNumber != self.run:
                continue
                
            [type, tbit, beam] = self.BBTool.GetNumber(event.data['region'])


            if not self.doItonce:
                self.filln    = event.data['fillnum']  
                self.nLS      = 0   
                self.nBx1     = event.data['BX1']        
                self.nBx2     = event.data['BX2']
                self.nBxc     = event.data['BXcoll']        
                self.rDur     = (event.data['t_stop']-event.data['t_start'])/60.
                self.nLB      = event.data['n_slices']
                self.tslice   = event.data['slice_length']/60.
                #self.prescale = event.data['presc_85']            
                
                for k in range(64):
                    self.presc_t[k] = event.data['presc_t'][k] 
   
                for k in range(128):
                    self.presc_a[k] = event.data['presc_a'][k]
                    
                self.doItonce=True
                    
                
            if beam==1:
                self.rates_B1[tbit-3]  = event.data['bb_rate'][0]
          
            if beam==2:
                self.rates_B2[tbit-3]  = event.data['bb_rate'][0]
   

    def ProcessStop(self):

        # Then we do the graphs

        text="Fill number: <a target='_blank'  class='urllink' href='https://cmswbm.web.cern.ch/cmswbm/cmsdb/servlet/FillReport?FILL="
        text_s = "%s%d' title='' rel='nofollow'>%d</a>.<br /><br />\n"%(text,self.filln,self.filln)            
        self.htm.write(text_s)
        text="Run duration (in min): %.1f<br />\n"%(self.rDur)
        self.htm.write(text)                  
        self.htm.write("Number of bunches:<br /></p>\n")
        text="<ul><li>Beam 1: <strong>%d</strong></li>\n"%(self.nBx1)
        self.htm.write(text)
        text="<li>Beam 2: <strong>%d</strong></li>\n"%(self.nBx2)
        self.htm.write(text)
        text="<li>Colliding: <strong>%d</strong></li></ul>\n"%(self.nBxc)
        self.htm.write(text)
        
        self.htm.write("<div class='vspace'></div>\n")
        self.htm.write("<h2>Overall normalized rates (in Hz/10<sup>11</sup>p)</h2>\n")

        self.htm.write("<p>Rate calculation is described on <a target='_blank'  class='urllink' href='http://sviret.web.cern.ch/sviret/Welcome.php?n=CMS.MIBMonitorHowTo' title='' rel='nofollow'>this page</a>. To get some info about the technical bits, have a look at the corresponding <a target='_blank'  class='urllink' href='https://twiki.cern.ch/twiki/bin/view/CMS/L1TechnicalTriggerBits' title='' rel='nofollow'>wiki</a>. The bit numbers given here corresponds to algo bits, which are described on <a target='_blank'  class='urllink' https://twiki.cern.ch/twiki/bin/view/CMS/GlobalTriggerAvailableMenus' title='' rel='nofollow'>this page</a>.</p>\n")

        self.htm.write("<p>In order to further remove albedo effect, we are only using the single bunches which are in front of bunch trains, of far enough from the last colliding bunch crossing (more than 0.2 microsecond (8 bunch crossings)).</p>\n")
        
        self.htm.write("<br>\n")
        self.htm.write("<table align='center' border='1' cellpadding='5' cellspacing='0' >\n")
        self.htm.write("<tr>\n")
        self.htm.write("<td id='GreyCell'  valign='top'> Bit number</td>\n")
        self.htm.write("<td id='GreyCell'  valign='top'> 3 (PreCollisions)</td>\n")
        self.htm.write("<td id='GreyCell'  valign='top'> 4 (BGas_BSC)</td>\n")
        self.htm.write("<td id='GreyCell'  valign='top'> 5 (BGas_HF)</td>\n")
        self.htm.write("<td id='GreyCell'  valign='top'> 8 (BHalo)</td>\n")
        self.htm.write("</tr>\n")

        self.htm.write("<tr>\n")
        self.htm.write("<td id='GreyCell'  valign='top'> <strong>L1*HLT prescale</strong></td>\n")

        for i in range(6):

            if i==3 or i==4:
                continue
            
            if self.presc_a[3+i]==1:
                self.htm.write("<td id='Code' align='center'  valign='top'> %d</td>\n"%self.presc_a[3+i])
            else:
                self.htm.write("<td id='RedCell' align='center'  valign='top'> %d</td>\n"%self.presc_a[3+i])

        self.htm.write("</tr>\n")
                
        self.htm.write("<tr>\n")
        self.htm.write("<td id='GreyCell'  valign='top'> <strong>BEAM1</strong></td>\n")

        for i in range(6):

            if i==3 or i==4:
                continue
            
            if self.rates_B1[i]>0.1:
                self.htm.write("<td id='Code' align='center'  valign='top'> %.2f</td>\n"%self.rates_B1[i])
            else:
                self.htm.write("<td id='BlackCell' align='center'  valign='top'> %.2f</td>\n"%self.rates_B1[i])

        self.htm.write("</tr>\n")
        self.htm.write("<tr>\n")
        self.htm.write("<td id='GreyCell'  valign='top'> <strong>BEAM2</strong></td>\n")

        for i in range(6):

            if i==3 or i==4:
                continue

            if self.rates_B2[i]>0.1:
                self.htm.write("<td id='Code' align='center'  valign='top'> %.2f</td>\n"%self.rates_B2[i])
            else:
                self.htm.write("<td id='BlackCell' align='center'  valign='top'> %.2f</td>\n"%self.rates_B2[i])

        self.htm.write("</tr>\n")
        
        self.htm.write("</table>\n")
        self.htm.write("<br>\n")

        self.htm.write("<div class='vspace'></div><h2>Rate as a function of the distance to the last collision (ALBEDO)</h2>\n")

        self.htm.write("<p>The rate is not supposed to depend on the distance between the bunch crossing considered and the last collision bunch crossing. If such a dependance is observed, it would mean that our trigger selection is contaminated by some non-beam activity.</p>\n")

        self.htm.write("<p>The following plot shows, for the events passing algo bit 6 (Interbunch_BSC), the number of events recorded as a function of the distance to the last colliding BX, measured in microseconds. Different cluster multiplicity cuts are applied. Interbunch bit is based on events recorded in quiet BXs (no beam at all).</p><br>\n")

        self.htm.write("<a target='_blank' class='urllink' href='bit_rate_run_%d_BCID.png'>\n"%(self.run))
        self.htm.write("<img id='fig2' src='bit_rate_run_%d_BCID.png'></a>\n"%(self.run))

        self.htm.write("<div id='FigTitle'>Rate evolution as a function of the distance to the last collision.</div>\n")

        
        self.htm.write("<div class='vspace'></div><h2>PIXEL mean charge versus Strips mean charge</h2>\n")

        self.htm.write("<p>The three following plots shows, for events passing algo bits 4 and 5, and events passing bit 5 and having technical bit 8, the mean charge of the recorded clusters in the pixel barrel, either versus the mean charge of the clusters recorded in the tracker inner barrel (TIB) (fig 1 to 3).</p><br>\n")

        self.htm.write("<div id='equation'>\n")
        self.htm.write("<table class='center'>\n")
        self.htm.write("<tbody>\n")
        self.htm.write("<tr>\n")
        self.htm.write("<td>\n")
        self.htm.write("<a target='_blank' class='urllink' href='Charge_2D_0_%d.png'>\n"%(self.run))
        self.htm.write("<img id='fig3' src='Charge_2D_0_%d.png' alt=''></a>\n"%(self.run))
        self.htm.write("<div id='FigTitle'>1. PIXB vs TIB mean cluster charge, for events passing bit 4.</div>\n")
        self.htm.write("</td>\n")
        self.htm.write("<td>\n")
        self.htm.write("<a target='_blank' class='urllink' href='Charge_2D_1_%d.png'>\n"%(self.run))
        self.htm.write("<img id='fig3' src='Charge_2D_1_%d.png' alt=''></a>\n"%(self.run))
        self.htm.write("<div id='FigTitle'>2. PIXB vs TIB mean cluster charge, for events passing bit 5.</div>\n")
        self.htm.write("</td>\n")
        self.htm.write("<td>\n")
        self.htm.write("<a target='_blank' class='urllink' href='Charge_2D_2_%d.png'>\n"%(self.run))
        self.htm.write("<img id='fig3' src='Charge_2D_2_%d.png' alt=''></a>\n"%(self.run))
        self.htm.write("<div id='FigTitle'>3. PIXB vs TIB mean cluster charge, for events passing bit 5 and tech.8.</div>\n")
        self.htm.write("</td>\n")
        self.htm.write("</tr>\n")
        self.htm.write("</tbody>\n")
        self.htm.write("</table>\n")


        self.htm.write("<p>The three following plots shows, for the same configuration the mean charge of the clusters recorded in the pixel barrel (fig 4 to 6).</p><br>\n")
        
        self.htm.write("<div id='equation'>\n")
        self.htm.write("<table class='center'>\n")
        self.htm.write("<tbody>\n")
        self.htm.write("<tr>\n")
        self.htm.write("<td>\n")
        self.htm.write("<a target='_blank' class='urllink' href='Charge_1D_0_%d.png'>\n"%(self.run))
        self.htm.write("<img id='fig3' src='Charge_1D_0_%d.png' alt=''></a>\n"%(self.run))
        self.htm.write("<div id='FigTitle'>4. PIXB mean cluster charge, for events passing bit 4.</div>\n")
        self.htm.write("</td>\n")
        self.htm.write("<td>\n")
        self.htm.write("<a target='_blank' class='urllink' href='Charge_1D_1_%d.png'>\n"%(self.run))
        self.htm.write("<img id='fig3' src='Charge_1D_1_%d.png' alt=''></a>\n"%(self.run))
        self.htm.write("<div id='FigTitle'>5. PIXB mean cluster charge, for events passing bit 5.</div>\n")
        self.htm.write("</td>\n")
        self.htm.write("<td>\n")
        self.htm.write("<a target='_blank' class='urllink' href='Charge_1D_2_%d.png'>\n"%(self.run))
        self.htm.write("<img id='fig3' src='Charge_1D_2_%d.png' alt=''></a>\n"%(self.run))
        self.htm.write("<div id='FigTitle'>6. PIXB mean cluster charge, for events passing bit 5 and tech.8..</div>\n")
        self.htm.write("</td>\n")
        self.htm.write("</tr>\n")
        self.htm.write("</tbody>\n")
        self.htm.write("</table>\n")


        self.htm.write("<p>The three following plots shows, for the same configuration the pixel vs HF asymmetry (fig 7 to 9).</p><br>\n")

        self.htm.write("<div id='equation'>\n")
        self.htm.write("<table class='center'>\n")
        self.htm.write("<tbody>\n")
        self.htm.write("<tr>\n")
        self.htm.write("<td>\n")
        self.htm.write("<a target='_blank' class='urllink' href='HF_vs_PIX_0_%d.png'>\n"%(self.run))
        self.htm.write("<img id='fig3' src='HF_vs_PIX_0_%d.png' alt=''></a>\n"%(self.run))
        self.htm.write("<div id='FigTitle'>7. HF vs PIX asymetry, for events passing bit 4.</div>\n")
        self.htm.write("</td>\n")
        self.htm.write("<td>\n")
        self.htm.write("<a target='_blank' class='urllink' href='HF_vs_PIX_1_%d.png'>\n"%(self.run))
        self.htm.write("<img id='fig3' src='HF_vs_PIX_1_%d.png' alt=''></a>\n"%(self.run))
        self.htm.write("<div id='FigTitle'>8. HF vs PIX asymetry, for events passing bit 5..</div>\n")
        self.htm.write("</td>\n")
        self.htm.write("<td>\n")
        self.htm.write("<a target='_blank' class='urllink' href='HF_vs_PIX_2_%d.png'>\n"%(self.run))
        self.htm.write("<img id='fig3' src='HF_vs_PIX_2_%d.png' alt=''></a>\n"%(self.run))
        self.htm.write("<div id='FigTitle'>9. HF vs PIX asymetry, for events passing bit 5 and tech.8..</div>\n")
        self.htm.write("</td>\n")
        self.htm.write("</tr>\n")
        self.htm.write("</tbody>\n")
        self.htm.write("</table>\n")
        self.htm.write("</div>\n")
                
        self.htm.write(" <div class='vspace'></div><h2>Rate evolution during run</h2>\n")
        self.htm.write("<p><br clear='all' /></p>\n")


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
        self.htm.write("</body>\n")
        self.htm.write("</html>\n")
        self.htm.close()


    def WriteInfo(self,bitname):


        
        text="<ul><li>Algo bit %s rate<br /></li></ul>\n"%(bitname)
        
        self.htm.write(text)
        
        self.htm.write("<div> \n")

        self.pixcuts=[0,3,10,30,100]


        self.htm.write("<div id='equation'>\n")
        self.htm.write("<table class='center'>\n")
        self.htm.write("<tbody>\n")
	self.htm.write("   <tr>\n")

        for i in range(5):

            self.htm.write("<td>\n")

            text="<a target='_blank' class='urllink' href='bit_%s_rate_run_%d_%d.png'>\n"%(bitname,self.run,i)
            self.htm.write(text)
            
            text="<img id='figd' src='bit_%s_rate_run_%d_%d.png'>\n"%(bitname,self.run,i)
            self.htm.write(text)

            text="<div id='FigTitle'>%d. Rate evolution during run with at least %d pixel cluster(s).</div>\n"%(i,self.pixcuts[i])
            self.htm.write(text)
            
            self.htm.write("</td>\n")

        self.htm.write("	   </tr>\n")
        self.htm.write("   </tbody>\n")
        self.htm.write("   </table>\n")   
        self.htm.write("   </div>\n")
        self.htm.write("<br />\n")
        self.htm.write("</div>\n")
        self.htm.write("<div class='vspace'></div>\n")
