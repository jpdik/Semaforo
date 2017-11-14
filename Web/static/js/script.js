$(document).ready(function () {
	$('ul.tabs').tabs();
	$('select').material_select();
	$('.modal').modal();
	$('.tooltipped').tooltip({ delay: 50 });

	String.prototype.format = function () {
		var args = arguments;
		return this.replace(/\{\{|\}\}|\{(\d+)\}/g, function (m, n) {
			if (m == "{{") { return "{"; }
			if (m == "}}") { return "}"; }
			return args[n];
		});
	};

	//Descoberta(Necessário para elementos dinamicos). Também funciona com elementos fixos.
	$(document).on("change", '#dep', function () {
		//desativa o campo parente de tempo aberto caso seja selecionado uma dependencia.
		if ($(this).val() == 0) {
			$(this).parents('.tempo').find('#tempo_aberto').prop("disabled", false);
			//Verifica se a quantidade de campos é zero, o que indica que ainda nao foi clonado nenhum campo novo de semaforo e com isso altera o campo fechado tbm
			if ($(this).parents('.criaSemaforo .row').length == 0)
				$('#tempo_fechado').prop("disabled", false);
		}
		else {
			$(this).parents('.tempo').find('#tempo_aberto').prop("disabled", true);
			//Verifica se a quantidade de campos é zero, o que indica que ainda nao foi clonado nenhum campo novo de semaforo e com isso altera o campo fechado tbm
			if ($(this).parents('.criaSemaforo .row').length == 0)
				$('#tempo_fechado').prop("disabled", true);
		}
	});

	$(document).on("click", '#confirmacao', function (e) {
		e.preventDefault();

		dados = new Object();

		dados.id = $(this).parents('.card').find("input[name=idSem]").val()

		$('#message').html('Tem certeza que deseja apagar o grupo de semáforos "' + $(this).parents('.card').find(".card-title").html() + '"?');

		$('#confirm').modal('open');
	});

	$(document).on("click", '#apagar', function (e) {
		e.preventDefault();

		deletar_Regra();
	});

	//Variavel para atribuição de timer para coleta de dados dos sensores de proximidade(função que ira obter os dados dos sensores).
	var obterdados;

	var dados;

	status = 0;

	function adiciona_CriaSemaforo() {
		$(".card-content .criaSemaforo:last").clone().appendTo(".criaSemaforos");
		$(".card-content .criaSemaforo:last").prop("disabled", false);
		$('select').material_select();
		$('.row .criaSExtra').hide();
	}

	function remove_CriaSemaforo() {
		if ($('.card-content .criaSemaforo').length > 1) {
			$('.card-content .criaSemaforo:last').remove();
			if ($('.card-content .criaSemaforo').length == 1)
				$('.row .criaSExtra').show();
		}
	}

	function mudar_status_semaforo(tipo, status) {
		if (status == "enable")
			$('.semaforos .left .semaforo .' + tipo).each(function () {
				$(this).removeClass("disable");
			});
		else
			$('.semaforos .left .semaforo .' + tipo).each(function () {
				$(this).addClass(status);
			});
	}

	function manutencao() {
		mudar_status_semaforo("stop", "disable");
		mudar_status_semaforo("warning", "enable");
		mudar_status_semaforo("go", "disable");
		status = 0;
	}

	function reiniciar() {
		mudar_status_semaforo("stop", "enable");
		mudar_status_semaforo("warning", "enable");
		mudar_status_semaforo("go", "enable");
		status = 1;
	}

	function editar(value) {
		console.log($('input[value=' + value + '').val());
	}

	function criar() {
		dados = new Object();

		//Nome do grupo
		dados.group_name = $("#group_name").val();

		//Lista para os semáforos
		dados.rules_semaforo = []

		dados.tempos_aberto = []

		dados.dependencias = []

		//se tiver somente um semáforo na regra, obtém se o tempo fechado tbm
		if ($(".criaSemaforo #tempo_aberto").length == 1)
			dados.tempo_fechado = $(".criaSExtra #tempo_fechado").val()

		var status = 0;

		//Interagindo com a lista de semáforos do front end, e colocando na lista
		//Caso necessário
		$(".criaSemaforo #lista option:selected").each(function () {
			//Verifica se um dado foi selecionado
			if ($(this).val() != 0) {
				//verifica se já existe o elemento na lista. Caso não, o insere
				if (dados.rules_semaforo.indexOf($(this).val()) == -1)
					dados.rules_semaforo.push($(this).val());
				else
					status = 1;
			}
			else
				status = 2;
		});

		//Interagindo com a lista de dependencias semáforos do front end, e colocando na lista
		//0 para sem dependencia e o ID da dependencia caso tenha
		$(".criaSemaforo #dep option:selected").each(function () {
			dados.dependencias.push($(this).val());
		});

		$(".criaSemaforo #tempo_aberto").each(function () {
			//Verifica se um dado foi selecionado
			dados.tempos_aberto.push($(this).val());
		});

		return status;
	}

	function limpar_campos() {
		dados.group_name = $("#group_name").val("");
		for (i = 0; i < $('.card-content .criaSemaforo').length; i++)
			remove_CriaSemaforo();
		$(".criaSemaforo #tempo_aberto:last").val("0");
		$("#lista").prop('selectedIndex', 0);
		$("#lista").material_select();
	}

	$("#add").click(function () {
		adiciona_semaforo();
	});

	$("#remove").click(function () {
		remove_semaforo();
	});

	$("#addCria").click(function () {
		adiciona_CriaSemaforo();
	});

	$("#removeCria").click(function () {
		remove_CriaSemaforo();
	});

	$("#manutencao").click(function () {
		manutencao();
	});

	$("#reiniciar").click(function () {
		reiniciar();
	});

	$("#editar").click(function () {
		console.log("ok");
		editar($(this).val());
	});

	$("#criar").click(function () {
		var status = criar();
		if (status == "0") {
			criar_Regra();
			Materialize.toast('Criado com sucesso!', 2000, '', function () { location.reload(); });
		}
		else if (status == 1)
			Materialize.toast('Não pode ter 2 semáforos iguais em um grupo!', 4000);
		else if (status == 2)
			Materialize.toast('Selecione ao menos 1 semáforo em cada categora!', 4000);
	});

	function criar_Regra() {
		$.ajax({
			url: window.location.href.substring(0, window.location.href.lastIndexOf('/')) + '/regra/criar',
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

	function deletar_Regra() {
		$.ajax({
			url: window.location.href.substring(0, window.location.href.lastIndexOf('/')) + '/regra/deletar',
			success: function (e) {
				var resp = JSON.parse(e);
				if (resp['status'] == 1)
					Materialize.toast('Regra deletada com sucesso!', 2000, '', function () { location.reload(); });
			},
			error: function (e) {
			},
			type: 'DELETE',
			data: JSON.stringify(dados),
			contentType: 'application/json;charset=UTF-8',
			cache: false,
			processData: false
		});
	}

	function obter_Semaforos() {
		$.ajax({
			url: window.location.href.substring(0, window.location.href.lastIndexOf('/')) + '/semaforos',
			success: function (e) {
				var resp = JSON.parse(e);
				if (resp['status'] == 1)
					configurar_Lista(resp['semaforos'])
			},
			error: function (e) {

			},
			type: 'GET',
			cache: false,
			contentType: false,
			processData: false
		});
	}

	function obter_Regras() {
		$.ajax({
			url: window.location.href.substring(0, window.location.href.lastIndexOf('/')) + '/regras',
			success: function (e) {
				var regras = JSON.parse(e);
				carregar_regras(regras);
			},
			error: function (e) {
				obter_Regras();
			},
			type: 'GET',
			cache: false,
			contentType: false,
			processData: false
		});
	}

	//obtém os dados dos sensores do veículo. Esta função cria um timer no javascript quando a conexão for realizada, e finalizada quando for desconectado.
	function obter_dados() {
		obterdados = setInterval(function () {
			if (status == 1) {
				mudar_status_semaforo("stop", "disable");
				mudar_status_semaforo("warning", "disable");
				mudar_status_semaforo("go", "enable");
				status++;
			}
			else if (status == 2) {
				mudar_status_semaforo("stop", "disable");
				mudar_status_semaforo("warning", "enable");
				mudar_status_semaforo("go", "disable");
				status++;
			}
			else if (status == 3) {
				mudar_status_semaforo("stop", "enable");
				mudar_status_semaforo("warning", "disable");
				mudar_status_semaforo("go", "disable");
				status = 1;
			}
		}, 1000);
	}

	function carregar_regras(regras) {
		console.log(regras);

		for (var i = 0; i < regras['semaforos'].length; i++) {
			$(".grupos .row").append($modeloCartao.format(regras['semaforos'][i]['id'], regras['semaforos'][i]['group_name']));

			for (var j = 0; j < regras['semaforos'][i]['rules_semaforo'].length; j++)
				$(".grupos .row .semaforos:last").append($modelosemaforo.format(regras['semaforos'][i]['rules_semaforo'][j]));
		}
	}

	obter_dados();
});
