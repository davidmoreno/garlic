var okicon
var runningicon
var erroricon

reload_results = function(){
	var pollingd=false;
	$.get('/results', function(data){
		running=false;
		var ul=$('#results')
		ul.html('')
		var lis=[]
		for (var k in data){
			var li=$('<li>')

			//var a=$('<a>').attr('href','/result/'+k)
			var a=$('<a>').attr('href','#'+k).click(function(id){ return function(){ showResult(id) } }(k))

			if (data[k].result==0)
				a.append(okicon.clone()).addClass("ok");
			else if (data[k].running){
				a.append(runningicon.clone()).addClass("running");
				running=true;
			}
			else
				a.append(erroricon.clone()).addClass("error");

			a.append(data[k].name)
			a.append('<br/>' + (new Date(Number(data[k].timestamp)*1000)) + '<br>Result: ' + data[k].result)
			
			li.append(a)
			
			li.attr('timestamp',data[k].timestamp)
			
			lis.push(li)
		}
		
		lis=lis.sort(function(a,b){ 
			return a.attr('timestamp') < b.attr('timestamp') ? 1 : -1
			})
		ul.append(lis)
		
		updateResult()
		
		if (running){
			$('[name=run]').attr('disabled','disabled')
		}
		else{
			$('[name=run]').removeAttr('disabled')
		}
		
		if (running && !pollingd)
			pollingd=setTimeout(reload_results, 1000);
		if (!running && pollingd){
			clearInterval(pollingd)
			pollingd=false;
		}
	},'json')
}

hideResult = function(){
	$('#overwindow').remove()
}

output_lock_to_bottom=true

showResult = function(id){
	hideResult()
	var ow=$('<div id="overwindow">').append( $('<div id="background">').click(hideResult) )
	var out=$('<div id="output">').attr('test',id)
	ow.append(out)
	$('body').append(ow)
	out.append( $('<a>').attr('href','/result/'+id).text('{% trans "Get RAW" %}') )
	out.on('scroll',function(e){
		output_lock_to_bottom=(out[0].scrollHeight - out.scrollTop() <= out.outerHeight()+10);
	})
	
	updateResult()
}

updateResult = function(id){
	if ($('#output').length==0)
		return
	$.get('/result/'+$('#output').attr('test'),function(data){
		var output=$('#output')
		var out=output.html('').append($('<pre>').text(data))
		if (output_lock_to_bottom)
			output[0].scrollTop = output[0].scrollHeight;
	},'text')
}

$(document).ready(function(){
	okicon=$("<span class='icon'>😎</span>");
	runningicon=$("<span class='icon'>♨</span>");
	erroricon=$("<span class='icon'>⛈</span>");

	reload_results();
	$('#reload').focus()
	var update_envvar = function(){
		var m=$(this)
		var val=m.val()
		if (val.length==0)
			val=m.attr('placeholder')
		$('[type=hidden][name='+m.attr('name')+']').val(val)
	}
	
	$('[type=text].envvar').each(update_envvar).change(update_envvar)
})
