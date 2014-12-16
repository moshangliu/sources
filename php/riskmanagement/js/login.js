var myForm;
var myCombo;

function load() 
{
    initMenu();
    initCombo();
}

function initMenu()
{

	myForm = new dhtmlXForm("myForm");
    myForm.loadStruct("xml/dhxform.xml");
    myForm.attachEvent("onButtonClick", login);
}


function login(name)
{
    var name = myForm.getInput('username').value
    var pw = myForm.getInput('password').value

    $.ajax(
    {
        url: 'php/checklogin.php',
        type: 'POST',
        data: {'name': name, 'pw': pw},
        dataType: 'html',
        success: function(valueReturnedByServer)
        {
            var res = eval('('+valueReturnedByServer+')');
                    
            if (res['success'] == true)
            {
                top.location='index.html';
            }
            else
            {
                alert(res['reason']);
            }
        }
    })
    
}