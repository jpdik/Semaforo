$(document).ready(function(){
	$('ul.tabs').tabs();
	$('select').material_select();
	$('.tooltipped').tooltip({delay: 50});

	//Variavel para atribuição de timer para coleta de dados dos sensores de proximidade(função que ira obter os dados dos sensores).
	var obterdados;

	var dados;

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
		$(".semaforos .left:last").remove();
	}

	function adiciona_CriaSemaforo(){
		$( ".card-content .criaSemaforo:last" ).clone().appendTo( ".criaSemaforos" );
		$('select').material_select();
	}

	function remove_CriaSemaforo(){
		if($('.card-content .criaSemaforo').length > 1)
			$('.card-content .criaSemaforo:last').remove();
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

	function criar(){
		dados = new Object();
		
		//Nome do grupo
		dados.group_name = $("#group_name").val();
		
		//Lista para os semáforos
		dados.rules_semaforo = []

		dados.tempos_semaforo = []

		var status = 0;

		//Interagindo com a lista de semáforos do front end, e colocando na lista
		//Caso necessário
		$(".criaSemaforo #lista option:selected").each(function(){
	        //Verifica se um dado foi selecionado
	        if($(this).val() != 0){ 
	        	//verifica se já existe o elemento na lista. Caso não, o insere
	        	if(dados.rules_semaforo.indexOf($(this).val()) == -1)
	        		dados.rules_semaforo.push($(this).val());
	        	else
	        		status = 1;
	        }
	        else
	        	status = 2;
	    });

		$(".criaSemaforo #tempo_semaforo").each(function(){
	        //Verifica se um dado foi selecionado
	        dados.tempos_semaforo.push($(this).val());
	    });

	    return status;
	}

	function limpar_campos(){
		dados.group_name = $("#group_name").val("");
		for(i = 0; i < $('.card-content .criaSemaforo').length; i++)
			remove_CriaSemaforo();
		$(".criaSemaforo #tempo_semaforo:last").val("0");
		$("#lista").prop('selectedIndex', 0);
		$("#lista").material_select();
	}

	$( "#add" ).click(function() {
	  adiciona_semaforo();
	});

	$( "#remove" ).click(function() {
	  remove_semaforo();
	});

	$( "#addCria" ).click(function() {
	  adiciona_CriaSemaforo();
	});

	$( "#removeCria" ).click(function() {
	  remove_CriaSemaforo();
	});

	$( "#manutencao" ).click(function() {
		manutencao();
	});

	$( "#reiniciar" ).click(function() {
		reiniciar();
	});

	$( "#criar" ).click(function() {
		var status = criar();
		if( status == "0"){
			criar_Regra();
			Materialize.toast('Criado com sucesso!', 4000,'', function(){limpar_campos()});
		}
		else if( status == 1)
			Materialize.toast('Não pode ter 2 semáforos iguais em um grupo!', 4000);
		else if( status == 2)
			Materialize.toast('Selecione ao menos 1 semáforo em cada categora!', 4000);
	});

	function configurar_Lista(semaforos){
		try{
			for(i = 0; i < semaforos.length; i++)
				$("#lista:last").append("<option value='"+semaforos[i]+"'>Semaforo "+semaforos[i]+"</option>");
			$('select').material_select();
		}catch (Exception){
			obter_Semaforos();
		} 
	}

	function criar_Regra(){
		$.ajax({
			url: window.location.href.substring(0, window.location.href.lastIndexOf('/'))+'/criar_regra',
			success: function (e) {
			},
			error: function (e) {
			},
				type: 'POST',
				data: JSON.stringify(dados),
            	contentType: 'application/json;charset=UTF-8',
				cache: false,
				processData: false
			});
	}	

	function obter_Semaforos(){
		$.ajax({
			url: window.location.href.substring(0, window.location.href.lastIndexOf('/'))+'/semaforos',
			success: function (e) {
				var resp = JSON.parse(e);
				configurar_Lista(resp['semaforos'])
			},
			error: function (e) {
				obter_Semaforos();
			},
				type: 'GET',
				cache: false,
				contentType: false,
				processData: false
			});
	}	

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
	obter_Semaforos();
});
