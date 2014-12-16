var menu
var mygrid
var myForm
var modifyForm
var dhxWins
var temp

function load()
{
    initMenu();
	temp = 0;
	//edit();
}


function initMenu()
{
    menu = new dhtmlXMenuObject("menu");
    menu.setIconsPath("dhtmlxSuite/dhtmlxMenu/samples/common/imgs/");
    menu.attachEvent("onClick", menuClick);
    menu.loadXML("xml/dhxmenu.xml", function() {});	
	myForm = new dhtmlXForm("myForm");
//	myForm.attachEvent("onButtonClick", edit);
}

function menuClick(id)
{   
    if (id == 'usermanage')
    {   temp = 1;
		mygrid = new dhtmlXGridObject('grid');
		mygrid.setImagePath("dhtmlxSuite/dhtmlxGrid/codebase/imgs/");
		mygrid.setSkin("dhx_skyblue");
	//	mygrid.attachEvent("onRowSelect", doOnRowSelected);
		mygrid.init(); 

		$.ajax(
				{
					url: 'php/userload.php',
					type: 'POST',
					//data: {'name': name, 'pw': pw},
					dataType: 'html',
					success: function(valueReturnedByServer)
					{
						var res = eval('('+valueReturnedByServer+')');
						if (res['success'] == true)
						{
							mygrid.parse(res['data']);	
							myForm.unload();
							myForm = new dhtmlXForm("myForm");
							myForm.attachEvent("onButtonClick", userinfoedit);							
							myForm.loadStruct("xml/usereditform.xml");	
						}
						else
						{
							alert(res['reason']);
						}
					}
				})

	}
	if(id == 'riskadd')
	{ 
		
		if(temp == 1)
		{ 
			myForm.unload();
			mygrid.destructor();
		}
		myForm = new dhtmlXForm("grid");
		myForm.attachEvent("onButtonClick", riskadd);							
		myForm.loadStruct("xml/riskadd.xml");	
	
	}
	
    else if (id == 'exit')
    {
		mygrid.destructor();
		myForm.unload();
    }
}
function riskadd()
{

}


function userinfoedit(id)
{
	if (id="modify")
	{
		dhxWins = new dhtmlXWindows();
		dhxWins.enableAutoViewport(false);
		dhxWins.attachViewportTo("grid");
		dhxWins.setImagePath("../dhtmlxSuite/dhtmlxWindows/codebase/imgs/");
		wedit=dhxWins.createWindow("wedit", 20, 30, 320, 240);
		wedit.setText("用户信息修改");

		modifyForm = new dhtmlXForm("modifyForm");
		modifyForm.loadStruct("xml/userinfomodify.xml");
		wedit.attachObject("modifyForm",true);
		
	}

}

