$(document).ready(function(){
	//Variavel para atribuição de timer para coleta de dados dos sensores de proximidade(função que ira obter os dados dos sensores).
	var obterdados;

	status = 0;

	var $modelosemaforo = "<div class='left'>" +
                            	"<div class='idsemaforo'>1</div>" +
                            	"<div class='semaforo'>" +
	                                "<div class='stop disable'></div>" +
	                                "<div class='warning'></div>" +
	                                "<div class='go disable'></div>" +
                            	"</div>" +
                        	  "</div>";

	function adiciona_semaforo(){
		$(".semaforos").append( $modelosemaforo );
	}

	function remove_semaforo(){
		$('.semaforos .left:last').remove();
	}

	function mudar_status_semaforo(tipo, status){
		if(status == "enable")
			$('.semaforos .left .semaforo .'+tipo).each(function(){
	        	$(this).removeClass("disable");
	    	});
		else
			$('.semaforos .left .semaforo .'+tipo).each(function(){
	        	$(this).addClass(status);
	    	});
	}

	function manutencao(){
		mudar_status_semaforo("stop", "disable");
		mudar_status_semaforo("warning", "enable");
		mudar_status_semaforo("go", "disable");
		status = 0;
	}

	function reiniciar(){
		mudar_status_semaforo("stop", "enable");
		mudar_status_semaforo("warning", "enable");
		mudar_status_semaforo("go", "enable");
		status = 1;
	}


	$( "#add" ).click(function() {
	  adiciona_semaforo();
	});

	$( "#remove" ).click(function() {
	  remove_semaforo();
	});

	$( "#manutencao" ).click(function() {
		manutencao();
	});

	$( "#reiniciar" ).click(function() {
		reiniciar();
	});

	//obtém os dados dos sensores do veículo. Esta função cria um timer no javascript quando a conexão for realizada, e finalizada quando for desconectado.
	function obter_dados(){
		obterdados = setInterval(function(){
			if(status == 1){
				mudar_status_semaforo("stop", "disable");
				mudar_status_semaforo("warning", "disable");
				mudar_status_semaforo("go", "enable");
				status++;
			}
			else if(status == 2){
				mudar_status_semaforo("stop", "disable");
				mudar_status_semaforo("warning", "enable");
				mudar_status_semaforo("go", "disable");
				status++;
			}
			else if(status == 3){
				mudar_status_semaforo("stop", "enable");
				mudar_status_semaforo("warning", "disable");
				mudar_status_semaforo("go", "disable");
				status = 1;
			}
		}, 1000);
	}

	obter_dados();
});
